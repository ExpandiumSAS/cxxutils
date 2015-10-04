#include <cstring>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include <linenoise/linenoise.h>

#include <cxxu/readline.hpp>
#include <cxxu/utils.hpp>

namespace cxxu {

readline::readline(
    std::string name,
    std::string prompt,
    const readline_repl_params& params
)
: name_(name),
continuation_(false),
params_(params),
interactive_(cxxu::interactive())
{
    configure_prompt(prompt);

    std::string history_dir = cxxu::home_directory() + "/." + name;
    std::string history_file_name = name + "-history";

    history_file(cxxu::fullpath(history_dir, history_file_name));
}

readline::~readline()
{}

void
readline::history_file(std::string file)
{
    if (!interactive_) {
        return;
    }

    history_file_ = file;

    // Load history
    cxxu::mkfilepath(history_file_);
    linenoiseHistoryLoad((char*) history_file_.c_str());
}

void
readline::set_prompt(std::string prompt)
{
    configure_prompt(prompt);
}

void
readline::save_to_history(const std::string& line)
{
    if (!interactive_) {
        return;
    }

    linenoiseHistoryAdd((char*) line.c_str());
    linenoiseHistorySave((char*) history_file_.c_str());
}

void
readline::configure_prompt(std::string prompt)
{
    prompt_ = prompt + "> ";

    if (prompt.size() > 3) {
        continuation_prompt_ = std::string(prompt.size() - 3, ' ');
    }

    continuation_prompt_ += "...> ";
}

char*
readline::make_prompt(void)
{
    return (char*) (
        continuation_
        ? continuation_prompt_.c_str()
        : prompt_.c_str()
    );
}

template <typename T>
void
readline::read_line(std::basic_istream<T>* is_ptr)
{
    char* tline;
    std::string line;

    end_of_input_ = false;
    bool new_input = false;

    while (!new_input) {
        line.clear();

        if (is_ptr != nullptr) {
            if (!std::getline(*is_ptr, line)) {
                end_of_input_ = true;
                break;
            }
        } else {
            tline = linenoise(prompt_.c_str());

            if (tline == NULL) {
                end_of_input_ = true;
                break;
            }

            line.assign(tline);
            std::free(tline);
        }

        if (
            cxxu::match(line, "^[\\s\\n]*$")
            &&
            !continuation_
        ) {
            // Empty line at the prompt ...
            // Maybe some user trying to make sure we're alive
            continue;
        } else {
            new_input = true;
        }
    }

    if (new_input) {
        input_ += line;
    }
}

void
readline::handle_line(input_handler ih, command_handler ch)
{
    if (input_ != "" && end_of_input_) {
        std::cout << "unterminated input: " << input_ << std::endl;
        rc_ = 1;
    } else if (end_of_input_) {
        rc_ = 0;
    } else if (
        params_.command != '\0' && input_[0] == params_.command
        &&
        ch != nullptr
    ) {
        cxxu::chomp(input_);
        save_to_history(input_);

        std::string command_line = input_.substr(1);
        input_.clear();
        auto list = cxxu::split("\\s+", command_line);

        std::string command;
        std::vector<std::string> args;

        if (list.size() > 0) {
            command = list[0];

            for (std::size_t i = 1; i < list.size(); i++) {
                args.push_back(list[i]);
            }
        }

        int crc = ch(command, args);

        if (crc < 0) {
            // Command requests we quit
            end_of_input_ = true;
            rc_ = 0;
        }
    } else {
        while (
            cxxu::extract_delimited(
                input_, user_input_, params_.delim, params_.quote
            )
        ) {
            save_to_history(user_input_ + params_.delim);
            ih(user_input_);
        }

        if (!cxxu::match(input_, "^[\\s\\n]*$")) {
            // Incomplete input, remaining data
            // ask some more
            continuation_ = true;
        } else {
            input_.clear();
            continuation_ = false;
        }
    }
}

int
readline::repl(input_handler ih, command_handler ch)
{
    std::istringstream* ptr = nullptr;
    return process(ptr, ih, ch);
}

template <typename T>
int
readline::process(T* source, input_handler ih, command_handler ch)
{
    rc_ = 0;

    while (!end_of_input_) {
        read_line(source);
        handle_line(ih, ch);
    }

    return rc_;
}


int
readline::process_text(
    const std::string& text,
    input_handler ih, command_handler ch
)
{
    std::istringstream iss(text);

    return process(&iss, ih, ch);
}

int
readline::process_file(
    std::ifstream& ifs,
    input_handler ih, command_handler ch
)
{
    return process(&ifs, ih, ch);
}

int
readline::process_file(
    const std::string& file,
    input_handler ih, command_handler ch
)
{
    std::ifstream ifs(file);

    if (!ifs.is_open()) {
        std::cerr << "can't open file " << file << std::endl;
        rc_ = 1;
    } else {
        process(&ifs, ih, ch);
    }

    return rc_;
}

} // namespace cxxu
