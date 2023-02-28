#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include <string>
#include <vector>

struct function_def {
    function_def(std::string n, std::string r, std::vector<std::string> args) : name(n), return_type(r), arg_types(args) {
        arg_list = write_arg_list(arg_types);
        args_as_params = write_args_as_params(arg_types);
        arg_types_only = write_arg_types_only(arg_types);
    }

    function_def(std::string decl) {
        parse(decl);
    }

    void parse(std::string decl) {
        // We're gonna do this the simplest way possible
        // Fortunately, syscalls usually use non-crazy signatures
        std::stringstream ss(decl);

        std::getline(ss,return_type, ' ');
        std::getline(ss,name, '(');

        std::string token;
        while (std::getline(ss, token, ',') ) {
            token.erase(token.find_last_not_of(" );\n\r\t")+1);
            arg_types.push_back(token);
        }


        arg_list = write_arg_list(arg_types);
        args_as_params = write_args_as_params(arg_types);
        arg_types_only = write_arg_types_only(arg_types);
    }

    std::string name;
    std::string return_type;
    std::vector<std::string> arg_types;

    std::string arg_list;
    std::string args_as_params;
    std::string arg_types_only;

    private:

        static std::string write_arg_list(std::vector<std::string> arg_types) {
            std::stringstream out;
            int counter = 0;
            for (const std::string& arg_type : arg_types) {
                out << arg_type << " _" << std::to_string(counter++);
                if (counter != arg_types.size())
                    out << ", ";
            }

            return out.str();
        }

        static std::string write_args_as_params(std::vector<std::string> arg_types) {
            std::stringstream out;

            int counter = 0;
            for (const std::string& arg_type : arg_types) {
                out << " _" << std::to_string(counter++);
                if (counter != arg_types.size())
                    out << ", ";
            }
            return out.str();
        }

        static std::string write_arg_types_only(std::vector<std::string> arg_types) {
            std::stringstream out;

            int counter = 0;
            for (const std::string& arg_type : arg_types) {
                out << arg_type;
                if (++counter != arg_types.size())
                    out << ", ";
            }
            return out.str();
        }
};

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ) {
    std::string result = format;

    for (std::string s : {args...}) {
        int pos = result.find("%s");
        if (pos == std::string::npos) { throw std::runtime_error("Mismatch between format string and number of args."); }
        result.replace(pos, 2, s);
    }

    return result;
}


int main () {
    std::vector<function_def> functions_to_wrap;

    std::string inputfile = "syscalls_to_wrap.txt";
    std::ifstream in;
    in.open(inputfile);
    std::string decl;
    while (std::getline(in, decl)) {
        functions_to_wrap.emplace_back(decl);
    }


    std::string out_filename = "../SyscallMock.hh";
    std::ofstream out;
    out.open (out_filename);

    std::string header = 
    "#include <iostream>\n"
    "#include <functional>\n"
    "#include <sys/types.h>\n"
    "#include <sys/select.h>\n"
    "#include <sys/socket.h>\n\n"
    "class SyscallMock;\n"
    "static SyscallMock * the_system_context;\n";

    out << header;

    out << "\n";

    out << "extern \"C\" {\n";

    out << "\t// Declarations for wrap and real functions\n\n";

    for (const function_def& f : functions_to_wrap) {
        const std::string real_decl_fmt = "extern %s __real_%s(%s);";
        const std::string wrap_decl_fmt = "%s __wrap_%s(%s);";

        out << "\t" << string_format(real_decl_fmt, f.return_type, f.name, f.arg_list) << "\n";
        out << "\t" << string_format(wrap_decl_fmt, f.return_type, f.name, f.arg_list) << "\n\n";
    }

    out << "}\n";

    out << "\n";

    // Write SyscallMock class
    std::string syscall_class = 
    "class SyscallMock {\n"
    "public:\n";

    out << syscall_class;

    // Write constructor 
    out << "\tSyscallMock () {\n";
    
    // Set everything to real_impl in constructor
    for (const function_def& f : functions_to_wrap) {        
        const std::string real_impl_call_fmt = "real_%s_impl();";
        out << "\t\t" << string_format(real_impl_call_fmt, f.name) << "\n";
    }
    out << "\t}\n\n";

    // Write set_all_real()
    out << "\tvoid set_all_real() {\n";
    for (const function_def& f : functions_to_wrap) {        
        const std::string real_impl_call_fmt = "real_%s_impl();";
        out << "\t\t" << string_format(real_impl_call_fmt, f.name) << "\n";
    }
    out << "\t}\n\n";

    // Write set_all_noop()
    out << "\tvoid set_all_noop() {\n";
    for (const function_def& f : functions_to_wrap) {        
        const std::string real_impl_call_fmt = "noop_%s_impl();";
        out << "\t\t" << string_format(real_impl_call_fmt, f.name) << "\n";
    }
    out << "\t}\n\n";

    // Write all the implementation for each syscall together
    for (const function_def& f : functions_to_wrap) {

        const std::string comment_fmt = "// %s Implementation";

        const std::string switcher_impl_fmt = "%s %s (%s) { return %s_impl(%s); }";
        const std::string register_impl_fmt = "void register_%s_impl (std::function<%s(%s)> impl) { %s_impl = impl; }";
        const std::string real_impl_fmt = "void real_%s_impl () { %s_impl = [](%s) -> %s { return __real_%s(%s); }; }";
        const std::string noop_impl_fmt = "void noop_%s_impl () { %s_impl = [](%s) -> %s { return 0; }; }";
        const std::string lambda_member_fmt = "std::function <%s(%s)> %s_impl;";

        out << "\n\t" << string_format(comment_fmt, f.name) << "\n";
        out << "\tpublic:\n";
        out << "\t" << string_format(switcher_impl_fmt, f.return_type, f.name, f.arg_list, f.name, f.args_as_params) << "\n";
        out << "\t" << string_format(register_impl_fmt, f.name, f.return_type, f.arg_list, f.name) << "\n";
        out << "\t" << string_format(real_impl_fmt, f.name, f.name, f.arg_list, f.return_type, f.name, f.args_as_params) << "\n";
        out << "\t" << string_format(noop_impl_fmt, f.name, f.name, f.arg_list, f.return_type) << "\n";
        out << "\tprivate:\n";
        out << "\t" << string_format(lambda_member_fmt, f.return_type, f.arg_types_only, f.name) << "\n";
    }

    // End of SyscallMock class
    out << "};\n\n";

    out << "// Wrap function definitions\n";
    
    // Write the actual __wrap_<name> statements
    for (const function_def& f : functions_to_wrap) {
        const std::string wrap_func_fmt = "%s __wrap_%s(%s) { return the_system_context->%s(%s); }";
        out << string_format(wrap_func_fmt, f.return_type, f.name, f.arg_list, f.name, f.args_as_params) << "\n";
    }


    out.close();

    std::string flags_filename = "../syscall_mock_linkflags.mk";
    std::ofstream flags;
    flags.open (flags_filename);
    
    flags << "export SYSCALL_MOCK_LINKFLAGS=-Wl,";

    const std::string flag_fmt = "--wrap=%s";
    int counter = 0;
    for (const function_def& f : functions_to_wrap) {
        flags << string_format(flag_fmt, f.name);
        if (++counter != functions_to_wrap.size())
            flags << ",";
    }

    flags.close();

    return 0;
}


/*********************************************************************
 *       Example of generated file with 1 syscall (socket)
 *********************************************************************

#include <iostream>
#include <functional>

extern "C" { 
    extern int __real_socket(int a1, int a2, int a3); 
    int __wrap_socket(int a1, int a2, int a3); 
}

class SyscallMock {
    public:
    SyscallMock () {
        real_socket_impl(); 
    }

    int socket (int domain, int type, int protocol) {
        return socket_impl(domain, type, protocol);
    }

    void register_socket_impl(std::function<int(int, int, int)> impl) {
        socket_impl = impl;
    }

    void real_socket_impl() {
        socket_impl = [](int domain, int type, int protocol) -> int {
            return __real_socket(domain, type, protocol);
        };
    }

    private:    
    std::function<int(int, int, int)> socket_impl;
};

class SyscallMock;
static SyscallMock * the_system_context;

int __wrap_socket(int a1, int a2, int a3) { return the_system_context->socket(a1,a2,a3);}

*********************************************************************/