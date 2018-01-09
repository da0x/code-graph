//
//  main.cpp
//  dia
//
//  Created by Daher Alfawares on 11/15/17.
//  Copyright © 2017 Daher Alfawares. All rights reserved.
//

/* REMOVE */

#include <iostream>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <fstream>
#include <set>
#include <list>
#include <sstream>

namespace space {
    class object {
    public:
        class sub_object {
        };
    };
    
    class module : object {
        std::string filename;
        module(std::string filename): filename(filename) {
            object::sub_object foo;
        }
    };
}



using std::vector;
using std::string;
vector<string> split(std::string input, std::string expression){
    boost::regex expr {expression};
    boost::regex_token_iterator<std::string::iterator>
    it{input.begin(), input.end(), expr, -1};
    boost::regex_token_iterator<std::string::iterator> end;
    vector<string> v;
    while (it != end)
    {
        v.push_back(*it++);
    }
    
    return v;
}

vector<string> find_all(std::string input, std::string expression){
    boost::regex expr {expression};
    boost::regex_token_iterator<std::string::iterator>
    it{input.begin(), input.end(), expr, 1};
    boost::regex_token_iterator<std::string::iterator> end;
    vector<string> v;
    while (it != end)
    {
        v.push_back(*it++);
    }
    return v;
}

std::string find(std::string input, std::string expression){
    boost::regex expr {expression};
    boost::smatch what;
    if (boost::regex_search(input, what, expr))
    {
        return what[0];
    }
    return "";
}

namespace compiler {
    using std::string;
    
    class regex {
        string in;
    public:
        regex(string in):in(in){}
        bool match(string expression){
            boost::regex expr {expression};
            return boost::regex_match(in, expr);
        }
        string find(string expression,int group){
            boost::regex expr {expression};
            boost::smatch what;
            if (boost::regex_search(in, what, expr))
            {
                return what[group];
            }
            return "";
        }
        vector<string> find_all(string expression){
            boost::regex expr {expression};
            boost::regex_token_iterator<string::iterator>
            it{in.begin(), in.end(), expr, 1};
            boost::regex_token_iterator<string::iterator> end;
            vector<string> v;
            while (it != end)
            {
                v.push_back(*it++);
            }
            return v;
        }
    };
              
    namespace strip {
        string replace(string in, string fmt, string with){
            auto e = boost::regex {fmt};
            return boost::regex_replace(in,e,with);
        }
        string remove(string fmt, string in){
            return replace(in,fmt,"");
        }
        string strings(string in){
            auto e = boost::regex {};
            auto o = remove("\\\\\"",in);
            
            auto r = boost::regex {"\".*?\""};
            return boost::regex_replace(o,r,"\"\"");
        }
        string line_comments(string in) {
            auto o = boost::match_not_dot_newline | boost::match_default;
            auto r = boost::regex {"\\/\\/.*"};
            return boost::regex_replace(in, r, "", o);
        }
        string comments(string in){
            auto r = boost::regex {"\\/\\*.*\\*\\/"};
            return boost::regex_replace(in,r,"");
        }
        string empty_lines(string in){
            auto r = boost::regex {"\\n\\s*\\n"};
            return boost::regex_replace(in,r,"\n");
        }
        string includes(string in){
            auto o = boost::match_not_dot_newline | boost::match_default;
            auto r = boost::regex {"\\s*#.*"};
            return boost::regex_replace(in,r,"",o);
        }
        string whitespace(string in){
            auto r = boost::regex {"[\\s\\t\\n]+"};
            in = boost::regex_replace(in,r," ");
            
            auto b = boost::regex {"^[\\s]"};
            in = boost::regex_replace(in,b,"");
            
            auto e = boost::regex {"[\\s]$"};
            return boost::regex_replace(in,e,"");
        }
        string semicolons(string in){
            auto r = boost::regex {";"};
            return boost::regex_replace(in,r," ");
        }
        string special_cases(string in){
            auto b = boost::regex {"class func"};
            in = boost::regex_replace(in,b,"func");
            return in;
        }
    }
    
    
    string statements(string in){
        auto r0 = boost::regex
        {"[\\\"\\\'\\^<>\\$#\\(\\)\\+\\-\\,\\.:\\;\\[\\]\\*\\&\\%\\!\\=\\/\\\\\\?]"};
        in = boost::regex_replace(in,r0," ");
        auto r1 = boost::regex {"}"};
        in = boost::regex_replace(in,r1," } ");
        auto r2 = boost::regex {"{"};
        in = boost::regex_replace(in,r2," { ");
        in = strip::whitespace(in);
        return in;
    }
    
    string stage(string in){
        in = compiler::strip::strings(in);
        in = compiler::strip::line_comments(in);
        in = compiler::strip::comments(in);
        in = compiler::strip::includes(in);
        in = compiler::strip::semicolons(in);
        in = compiler::strip::whitespace(in);
        in = compiler::strip::special_cases(in);
        in = compiler::statements(in);
        std::cout << "--" << in << std::endl;
        return in;
    }
    
    class node {
    public:
        string name;
        std::vector<string> is;
        std::vector<node> children;
        std::set<string> statements;
        node(){}
        node(const node& other){
            *this = other;
        }
        node(const node&& other) noexcept{
            *this = other;
        }
        node(string name, std::istringstream& in):name(name){
            this->read_name();
            this->compile(in);
        }
        node& operator=(const node& other){
            this->name = other.name;
            this->is = other.is;
            this->children = other.children;
            this->statements = other.statements;
            return *this;
        }
        node& operator=(node&& other) noexcept {
            this->name = std::move(other.name);
            this->is = std::move(other.is);
            this->children = std::move(other.children);
            this->statements = std::move(other.statements);
            return *this;
        }
        void read_name(){
            typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
            tokenizer tok{name};
            int count = 0;
            for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it){
                if(count++ == 0){
                    name = *it;
                } else {
                    is.push_back(*it);
                }
            }
        }
        void compile(std::istringstream& in){
            std::string i;
            std::stringstream statement;
            while(in >> i){
                statement << " " << i;
                std::cout
                << "Name: "
                << name
                << " | statement = "
                << statement.str()
                << std::endl;
                auto exp = regex(statement.str());
                if(exp.match(".*?\\s(namespace|class|struct|protocol)\\s.*{"))
                {
                    string child = exp.find(".*?\\s(namespace|class|struct|protocol)\\s(.*){",2);
                    std::cout
                    << " -> "
                    << child
                    << std::endl;
                    statement.str("");
                    
                    children.push_back(node(child,in));
                    
                    std::cout
                    << " <- "
                    << child
                    << std::endl;
                    continue;
                }
                if(exp.match(".*?{.*?")){
                    append(statement.str());
                    statement.str("");
                    children.push_back(node("+",in));
                    continue;
                }
                if(exp.match(".*?}.*?")){
                    append(statement.str());
                    return;
                }
            }
        }
        void append(std::string s){
            typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
            tokenizer tok{s};
            for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it){
                statements.insert(*it);
            }
        }
        std::set<string> all_statements() const {
            auto s = this->statements;
            for(auto c:children){
                if(c.is_class()) continue;
                for(auto sc:c.all_statements()){
                    s.insert(sc);
                }
            }
            return s;
        }
        string level(int i){
            std::stringstream s;
            for(auto j=0; j<i; j++){
                s << "   |";
            }
            return s.str();
        }
        void print(int i=0){
            if(this->is_class()){
                std::cout << level(i) << name;

                if(!this->is.empty()){
                    std::cout << " \u21fe";
                    for(auto str:is){
                        std::cout << " " << str;
                    }
                }
                std::cout << std::endl;
            }
            for(auto c:children){
                c.print(i+1);
            }
        }
        bool is_class() const {
            return name != "+";
        }
    };
}

namespace test {
    std::string pretty(std::string input){
        boost::regex new_lines {"\n"};
        return boost::regex_replace(input,new_lines,"+");
    }
    template<typename func> bool o(func f,std::string in, std::string expected){
        auto out = f(in);
        auto result = out == expected;
        std::cout << (!result ? "\u00D7 " : "\u2713 ")
        << "Test case: " << pretty(in) << " -> " << pretty(out) << std::endl;
        return result;
    }
    void all(){
        using namespace compiler;
        assert(compiler::strip::remove("a","abc")=="bc");
        assert(compiler::strip::remove("\\\\\"","a\\\"bc")=="abc");
        assert(o(compiler::strip::line_comments, "abc", "abc"));
        assert(o(compiler::strip::line_comments, "abc//1", "abc"));
        assert(o(compiler::strip::line_comments, "abc//123456a{bcdeABCDE", "abc"));
        assert(o(compiler::strip::line_comments, "abc//h\n//de", "abc\n"));
        assert(o(compiler::strip::line_comments, "//h\nabc//de", "\nabc"));
        assert(o(compiler::strip::comments, "a/**/b", "ab"));
        assert(o(compiler::strip::comments, "a/*remove*/b", "ab"));
        assert(o(compiler::strip::comments, "a/*\n*/b", "ab"));
        assert(o(compiler::strip::strings, "", ""));
        assert(o(compiler::strip::strings, "\"\\\"\\\"\"", "\"\""));
        assert(o(compiler::strip::strings, "\"abc\"", "\"\""));
        assert(o(compiler::strip::strings, "\"\\\"a\\\"\"", "\"\""));
        assert(o(compiler::strip::strings, "\"\\\"\"", "\"\""));
        assert(o(compiler::strip::empty_lines, "ab", "ab"));
        assert(o(compiler::strip::empty_lines, "a\n\nb", "a\nb"));
        assert(o(compiler::strip::empty_lines, "a\n\t \nb", "a\nb"));
        assert(o(compiler::strip::includes, "#abce", ""));
        assert(o(compiler::strip::includes, "##abc", ""));
        assert(o(compiler::strip::includes, " #abc", ""));
        assert(o(compiler::statements, "a{}", "a { }"));
        assert(o(compiler::statements, "a{}{}", "a { } { }"));
        assert(o(compiler::statements, "a{b{}}", "a { b { } }"));
        assert(o(compiler::statements, "a{b{}c{}}", "a { b { } c { } }"));
        assert(o(compiler::statements, "a(b())", "a b"));
        assert(o(compiler::statements, "+-)(*&^%$#!/<>\\:,./?", ""));
        assert(o(compiler::strip::whitespace, "'\n '", "' '"));
        assert(o(compiler::strip::whitespace, "' \t'", "' '"));
        assert(o(compiler::strip::special_cases, "class func abc", "func abc"));
        std::string s =
        "namespace a {"
        "    class a1 {"
        "        struct a12 {"
        "            d.e;"
        "        };"
        "    };"
        "    class d1 : a1 {"
        "        struct e {"
        "        @interface f"
        "        @end"
        "            class g {"
        "            }"
        "        }"
        "    };"
        "}";
        s = compiler::stage(s);
        std::istringstream code(s);
        auto node = compiler::node("+",code);
        node.print();
    }
}

namespace output {
    class neo4j {
    public:
        const compiler::node &root;
        neo4j(const compiler::node& root):root(root){
        }
        
        typedef std::map<string,compiler::node> node_map;
        node_map classes;
        void print(){
            const compiler::node& n = root;
            this->classes = flatten(n);
            this->create();
            this->connect();
        }
        node_map flatten(const compiler::node& n) const {
            node_map v;
            node_map v_children;
            if(n.is_class()){
                v[n.name] = n;
            }
            for(auto child:n.children){
                auto flat_child = flatten(child);
                for(auto c:flat_child){
                    v[c.first] = c.second;
                }
            }
            return v;
        }
        void create(){
            for(auto c:this->classes){
                std::cout << "create ("<<c.first<<":Class {name:\""<<c.first<<"\"})" << std::endl;
            }
        }
        void connect(){
            for(auto s:this->classes){
                for(auto d:s.second.is){
                    if(classes.find(s.first)!=classes.end()){
                        std::cout << "create ("<<s.first<<")-[:is]->("<<d<<")" << std::endl;
                    }
                }
            }
            for(auto s:this->classes){
                auto dependencies = s.second.all_statements();
                for(auto dependency:dependencies){
                    if(dependency.empty())
                        continue;
                    if(classes.find(dependency)!=classes.end()){
                        std::cout
                        << "create ("<<s.first<<")-[:depends_on]->("<<dependency<<")" << std::endl;
                    }
                }
            }
        }
    };
}

int main(int argc, const char * argv[]) {
    test::all();
    //return 0;
   
    std::cout << argv[1] << std::endl;
    std::ifstream source;
    source.open(argv[1]);
    std::stringstream lines;
    std::string line;
    if (source.is_open()) {
        while ( std::getline( source, line ) ) {
            lines << line << std::endl;
        }
    }
    
    string s = lines.str();
    s = compiler::stage(s);
    std::istringstream code(s);
    
    auto node = compiler::node("+",code);
    node.print();
    
    output::neo4j neo4j(node);
    neo4j.print();

    return 0;
}
