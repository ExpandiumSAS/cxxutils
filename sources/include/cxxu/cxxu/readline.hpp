#ifndef __CXXU_READLINE_H__
#define __CXXU_READLINE_H__

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <fstream>

#include <timevault/config.h>

#include <cxxu/utils.hpp>

namespace cxxu {

struct readline_repl_params {
    readline_repl_params()
    : delim(';'),
    quote('\0'),
    command('\0')
    {}

    readline_repl_params(const readline_repl_params& other)
    : delim(other.delim),
    quote(other.quote),
    command(other.command)
    {}

    readline_repl_params(char d, char q = '\0', char c = '\0')
    : delim(d),
    quote(q),
    command(c)
    {}

    char delim;
    char quote;
    char command;
};

class TIMEVAULT_API readline {
public:
    typedef std::function<int(const std::string&)> input_handler;
    typedef std::function<
        int(const std::string&, const std::vector<std::string>&)
    > command_handler;

    readline(
        std::string name,
        std::string prompt,
        const readline_repl_params& params = readline_repl_params()
    );
    ~readline();

    void history_file(std::string file);
    void set_prompt(std::string prompt);

    int repl(input_handler ih, command_handler ch = nullptr);
    int process_text(
        const std::string& text,
        input_handler ih, command_handler ch = nullptr
    );
    int process_file(
        const std::string& file,
        input_handler ih, command_handler ch = nullptr
    );

private:
    template <typename T = char>
    void read_line(std::basic_istream<T>* is_ptr);

    void handle_line(input_handler ih, command_handler ch);
    void save_to_history(const std::string& line);

    void configure_prompt(std::string prompt);
    char* make_prompt(void);
    unsigned char complete(int ch);

    template <typename T>
    int
    process(T* source, input_handler ih, command_handler ch);

    int process_file(
        std::ifstream& ifs,
        input_handler ih, command_handler ch = nullptr
    );

    std::string name_;
    bool continuation_;
    readline_repl_params params_;
    bool interactive_;
    std::string prompt_;
    std::string continuation_prompt_;
    std::string history_file_;
    std::vector<std::string> history_;
    std::string input_;
    std::string user_input_;
    bool end_of_input_;
    int rc_;
};

} // namespace cxxu

#endif // __CXXU_READLINE_H__
