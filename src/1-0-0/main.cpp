#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <exception>
#include <cstdlib>
#include <iterator>
#include <algorithm>

using namespace std;

// flags

bool REGISTER_PRINT_MODE = false;
bool START_AND_STOP_MODE = false;
bool SECTION_PRINT_MODE = false;
bool NO_SYSTEM_REGISTERS = false;
bool NO_RETURN_REGISTERS = false;
bool PRINT_CMDLINE_INPUTS = false;
bool PRINT_PROC_LOG = false;
int NUMBER_OF_SYSTEM_REGISTERS = 8;

// colors

string text_RED_COLOR = "\033[31m";
string text_YELLOW_COLOR = "\033[33m";
string text_BLUE_COLOR = "\033[36m";
string text_GREEN_COLOR = "\033[32m";
string text_DEFAULT_COLOR = "\033[39m";

// errors

void INVALID_LABEL_NAME_ERROR(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " is not a valid label name for a function nor for a ext_set";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void NO_SOURCE_PASSED() {
    cout << text_RED_COLOR;
    cout << "error : " << text_YELLOW_COLOR;
    cout << " no source passed to interpreter";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(string s, string type) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " trying to access to register asking not for it's type, that in fact is ";
    cout << text_BLUE_COLOR << "'" << type << "'";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void INVALID_OSTREAM_TO_UNSET_REGISTER(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " trying to access as ostream to register that wasn't set before";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

string vector_to_string(vector<string> * v) {
    string s_ = "";
    if (v->size()>0)
        s_ += v->at(0);
    for(long unsigned int i=1; i<v->size(); i++)
        s_ += " "+v->at(i);
    return s_;
};

void INVALID_SYNTAX_IN_DOT_DATA(vector<string> * v) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << vector_to_string(v);
    cout << "'" << text_YELLOW_COLOR;
    cout << " syntax not recognized in .data";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void INVALID_SYNTAX_IN_PROCEDURE(vector<string> * v, string label) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << vector_to_string(v);
    cout << "'" << text_YELLOW_COLOR;
    cout << " syntax not recognized in procedure '"<<label<<"'";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void CANNOT_FIND_PROCEDURE(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " couldn't find this procedure";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void CANNOT_FIND_FILE(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " couldn't find this file";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void CANNOT_FIND_QUERY(int ID) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << ID << "'" << text_YELLOW_COLOR;
    cout << " couldn't find a proc query for this ID";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void CANNOT_FIND_REGISTER(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " couldn't find this register";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void CANNOT_CONVERT_TO_INT(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " couldn't convert this to numeric value";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void INVALID_EXT_ERROR(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " couldn't recognize this extension";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void MISSING_EXT_ERROR(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " couldn't find any extension";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void ALREADY_DECLARED_REGISTER(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " this register was already declared";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void EXPECTED_ARGUMENT_AFTER_OPTION(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " expected argument after this option";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void OPTION_NOT_RECOGNIZED(string s) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << s << "'" << text_YELLOW_COLOR;
    cout << " option not recognized";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

void INT_DIVISION_BY_ZERO(vector<string> * v, string label) {
    cout << text_RED_COLOR;
    cout << "error : " << text_BLUE_COLOR << "'";
    cout << vector_to_string(v);
    cout << "'" << text_YELLOW_COLOR;
    cout << " int division by 0 in procedure '"<<label<<"'";
    cout << text_DEFAULT_COLOR << endl;
    exit(1);
};

// constants

string RETURN_STATEMENT = "ret";
string SECTION_MARKER = "section";

// string formatting

void trim_string(string * line) {
    while( (line->size() > 0) && (line->front() == ' ' || line->front() == '\n' || line->front() == '\t') )
        line->erase(line->begin());
    while( (line->size() > 0) && (line->back() == ' ' || line->back() == '\n' || line->back() == '\t') )
        line->erase(line->end()-1);
};

void esclude_comments(string * line) {
    for(long unsigned int i=0; i<line->size(); i++) {
        if (line->at(i) == ';') {
            *line = line->substr(0,i);
            break;
        };
    };
};

// io-filestream

void read_whole_file(string name, vector<string>* vct) {
    ifstream file; file.open(name);
    string line; long unsigned int control_num = vct->size();
    while(getline(file,line)) {          // obtains the line
        trim_string(&line);              // trims the line
        esclude_comments(&line);         // finds and cuts out comments
        if (line.size() > 0)             // check for not-empty line
            vct->push_back(line);        // inserts the string in the vct
    };
    file.close();
    if (vct->size() == control_num)
        CANNOT_FIND_FILE(name);
};

// io-stream
template<class T>
void print_vector(vector<T> * vct, int tabs=1) {
    cout << string(tabs-1,'\t') << "Length of vector : " << vct->size() << " {" << endl;
    if (vct->size()>0) {
        for( auto element = vct->begin(); element != vct->end(); ++element )
            cout << string(tabs,'\t') << *element << endl;
    };
    cout << string(tabs-1,'\t') << "}" << endl;
};

// section class

class Section {
    private:
        vector<vector<string>> instructions {};
        string label="";
    public:
        //
        void insert_instruction(vector<string> * inst) {
            instructions.push_back(*inst);
        };
        
        void parse_instruction_string(string * line, vector<string> * inst) {
            string buffer="";
            bool not_in_string = true;
            for(char c : *line) {
                if ( not_in_string && ( (c == ',') || (c == ' ') ) ) {
                    if (buffer.size()>0) {
                        inst->push_back(buffer);
                        buffer = "";
                    };
                } else if (c == '"') {
                    if (not_in_string)
                        not_in_string = false;
                    else
                        not_in_string = true;
                    buffer += c;
                } else {
                    buffer += c;
                };
            };
            if (buffer.size()>0)
                inst->push_back(buffer);
        };
        
        void parse_section(vector<string> * sect) {
            vector<string> tmp {};
            for(auto line=sect->begin(); line!=sect->end(); ++line) {
                this->parse_instruction_string(&(*line),&tmp);
                this->insert_instruction(&tmp);
                tmp = {};
            };
        };
        
        void parse_and_insert(string * line) {
            vector<string> tmp {};
            this->parse_instruction_string(line,&tmp);
            this->insert_instruction(&tmp);
        };
        
        void print_section(int tabs=1) {
            cout << string(tabs-1,'\t') << "Length of section ("<<label<<") : " << instructions.size() << " {" << endl;
            if (instructions.size()>0)
            for(auto i=instructions.begin(); i!=instructions.end(); ++i) {
                print_vector(&(*i),tabs+1);
            };
            cout << string(tabs-1,'\t') << "}" << endl;
        };
        
        string getLabel() {
            return label;
        };
        // construct
        Section() {
            
        };
        
        Section(string L) {
            label = L;
        };
        // bools attrs
        bool is_a_action_list() {
            if (label.front() == '.')
                return false;
            if (label.front() == '_')
                return true;
            INVALID_LABEL_NAME_ERROR(label);
            return true; // solo per far stare zitto il compilatore
        };
        bool is_a_ext_set() {
            if (label.front() == '.')
            return true;
            if (label.front() == '_')
            return false;
            INVALID_LABEL_NAME_ERROR(label);
            return true; // solo per far stare zitto il compilatore
        };
        
        bool is_a_procedure() {
            if (this->is_a_action_list() && !(this->returns_something()))
                    return true;
            else 
            return false;
        };
        
        bool is_a_function() {
            if (this->is_a_action_list() && this->returns_something())
            return true;
            else
            return false;
        };
        
        bool returns_something() {
            for(auto vct = instructions.begin(); vct!=instructions.end(); ++vct) {
                if (vct->front() == RETURN_STATEMENT)
                    return true;
            };
            return false;
        };
        // operators
        bool operator<(Section& B) {
            if (this->is_a_ext_set() && !(B.is_a_ext_set()))
                return true;
            if (B.is_a_ext_set() && !(this->is_a_ext_set()))
                return false;
            //
            int length;
            if (label.size() <= B.getLabel().size())
                length = label.size();
            else
                length = B.getLabel().size();
            //
            for(int i=1; i<length; i++) {
                if ( ((unsigned int)(label[i])) < ((unsigned int)(B.getLabel()[i])) ) {
                    return true;
                } else if ( ((unsigned int)(label[i])) > ((unsigned int)(B.getLabel()[i])) ) {
                    return false;
                };
            };
            return false;
        };
        
        void operator+(Section& B) {
            for( auto inst = B.begin(); inst != B.end(); ++inst ) {
                instructions.push_back((*inst));
            };
        };
        
        // pointers
        vector<vector<string>>::iterator begin() {
            return instructions.begin();
        };
        
        vector<vector<string>>::iterator end() {
            return instructions.end();
        };
};

// class Register
class Register {
    private:
        string label_name="";
        int numeric_value=0;
        string string_value="";
        int type_of_val_held=-1;
    public:
        // invulnerablility and scope
        bool invulnerable=false;
        string scope="";
        // reset button
        void reset() {
            numeric_value = 0;
            string_value = "";
            type_of_val_held = -1;
        };
        // construct
        Register(bool inv=false) {
            invulnerable = inv;
        };
        
        Register(string label, bool inv=false) {
            label_name = label;
            invulnerable = inv;
        };
        
        Register(string label, int V, bool inv=false) {
            label_name = label;
            numeric_value = V;
            type_of_val_held = 0;
            invulnerable = inv;
        };
        
        Register(string label, string V, bool inv=false) {
            label_name = label;
            string_value = V;
            type_of_val_held = 1;
            invulnerable = inv;
        };
        // read value
        void access_to_value(int * n) {
            if (type_of_val_held == -1) {
                *n = numeric_value;
            } if (type_of_val_held == 0) {
                *n = numeric_value;
            } else {
                INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(label_name,"string");
            };
        };
        
        void access_to_value(string * s) {
            if (type_of_val_held == -1) {
                *s = string_value;
            } if (type_of_val_held == 1) {
                *s = string_value;
            } else {
                INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(label_name,"numeric");
            };
        };
        // write value
        void write_value(int n) {
            numeric_value = n;
            type_of_val_held = 0;
        };
        
        void write_value(string s) {
            string_value = s;
            type_of_val_held = 1;
        };
        // label_name and type_of_val
        string getLabel() {
            return label_name;
        };
        
        int getType() {
            return type_of_val_held;
        };
        // operators
        void operator=(Register * R) {
            if (R->getType() == 0) {
                type_of_val_held = 0;
                R->access_to_value(&numeric_value);
            } else if (R->getType() == 1) {
                type_of_val_held = 1;
                R->access_to_value(&string_value);
            } else {
                INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(R->getLabel(),"unknown");
            };
        };
};

// section ext

void parse_intestation_of_section(string * line, string * label) {
    for(long unsigned int i=0; i<line->size(); i++) {
        if ( line->at(line->size()-1-i) == ' ' ) {
            *label = line->substr(line->size()-i,i);
        };
    };
};

bool is_section_marker(string * line) {
    if (line->size()>7) {
        if (line->substr(0,8) == (SECTION_MARKER+" ")) {
                             // section tag must  be separated
            return true;     // from label with a spacebar hit
        } else {
            return false;
        };
    } else {
        return false;
    };
};

void divide_in_sections(vector<string> * vct, vector<Section> * sct) {
    bool im_in_section = false; string label="";
    for ( auto str=vct->begin(); str != vct->end(); ++str ) {
        if (is_section_marker(&(*str))) {
            parse_intestation_of_section(&(*str),&label);
            sct->push_back(Section(label));
            im_in_section = true;
        } else {
            if (im_in_section) {
                sct->back().parse_and_insert(&(*str));
            };
            // ignore instructions out of sections
        };
    };
};

void print_sections(vector<Section> * sct, int tabs=1) {
    cout << string(tabs-1,'\t') << "Length of section list : " << sct->size() << " {" << endl;
    if (sct->size()>0)
    for(auto s = sct->begin(); s!=sct->end(); ++s) {
        s->print_section(tabs+1);
    };
    cout << string(tabs-1,'\n') << endl;
};

void find_what_each_section_is(vector<Section> * sct) {
    for(auto sec=sct->begin(); sec!=sct->end(); ++sec) {
        if (sec->is_a_function())
        cout << sec->getLabel() << " is a function." << endl;
    else if (sec->is_a_procedure())
        cout << sec->getLabel() << " is a procedure." << endl;
    else if (sec->is_a_ext_set())
        cout << sec->getLabel() << " is a ext_set." << endl;
    else
        cout << "typeof("<<sec->getLabel()<<") not recognized." << endl;
    };
};

bool is_Section_A_smaller_than_B(Section& a, Section& b) {
    if (a<b)
        return true;
    else
        return false;
};

void sort_sections(vector<Section> * v) {
    sort(v->begin(), v->end(), &(is_Section_A_smaller_than_B));
};

void merge_equals(vector<Section> * v) {
    // please note that v is already sorted linearly
    for(auto S=v->begin(); S!=v->end()-1; ++S) {
        if ( S->getLabel() == (S+1)->getLabel() ) {
            (*S)+(*(S+1));
            v->erase((S+1));
            --S;
        };
    };
};

// register ext

ostream& operator<<(ostream& out, Register * R) {
    switch(R->getType()) {
        case 0 : {
            int n;
            R->access_to_value(&n);
            out << n;
        }; break;
        case 1 : {
            string s;
            R->access_to_value(&s);
            out << s;
        }; break;
        default: INVALID_OSTREAM_TO_UNSET_REGISTER(R->getLabel());
    };
    return out;
};

string format_data_in_string(string data, int chars) {
    string res="";
    chars = chars - data.size();
    if (chars%2==0) {
        chars = chars/2;
        res += string(chars,' ') + data + string(chars,' ');
    } else {
        chars = chars/2;
        res += string(chars+1,' ') + data + string(chars,' ');
    };
    return res;
};

string define_register_type_by_code(int c) {
    string res="";
    switch(c) {
        case 0 : res="int"; break;
        case 1 : res="string"; break;
        default : res="unknown"; break;
    };
    return res;
};

string define_register_invulnerability(bool c) {
    string res="";
    if (c)
        res = "✅";
    else
        res = "❌";
    return res;
};

void print_register_inline(Register * regt, int chars_for_opt) {
    cout << "|";
    cout << format_data_in_string(regt->getLabel(),chars_for_opt) << "|";
    cout << format_data_in_string(define_register_type_by_code(regt->getType()),chars_for_opt) << "|";
    switch(regt->getType()) {
        case 0 : { int n; regt->access_to_value(&n); cout << format_data_in_string(to_string(n),chars_for_opt) << "|"; }; break;
        case 1 : { string n; regt->access_to_value(&n); cout << format_data_in_string(n,chars_for_opt) << "|"; }; break;
        default : cout << format_data_in_string("NaN",chars_for_opt) << "|"; break;
    };
    cout << format_data_in_string(regt->scope,chars_for_opt) << "|";
    cout << format_data_in_string(define_register_invulnerability(regt->invulnerable),chars_for_opt) << "|";
    cout << endl;
};

void print_registers(vector<Register> * regs,int chars_for_opt=35, string cmd="") {
    cout << "/" << string((chars_for_opt*5)+4,'-') << "\\" << endl;
    //
    if (cmd != "") {
        cout << "| using cmd : " << format_data_in_string(cmd,(chars_for_opt*5)-9) << "|" << endl;
        cout << "|" << string((chars_for_opt*5)+4,'-') << "|" << endl;
    }
    {
        cout << "|" << format_data_in_string("Label",chars_for_opt) << "|" << format_data_in_string("Type",chars_for_opt) << "|" << format_data_in_string("Value",chars_for_opt) << "|" << format_data_in_string("Scope",chars_for_opt) << "|" << format_data_in_string("Invulnerability",chars_for_opt) << "|" << endl;
        cout << "|" << string((chars_for_opt*5)+4,'-') << "|" << endl;
    };
    //
    for(auto regt=regs->begin(); regt!=regs->end(); ++regt)
        print_register_inline(&(*regt),chars_for_opt);
    //
    cout << "\\" << string((chars_for_opt*5)+4,'-') << "/" << endl;
};

// class Processor

class Processor {
    private:
        vector<Register> * ptr_to_registers;
    public:
        
        Processor() {
            
        };
        
        Processor(vector<Register> * regs) {
            ptr_to_registers = regs;
        };
        
        // queries
        void query__0() {
            Register * ra0; Register * ran; int nOf,argN; string argS;
            ra0 = this->find_register_for("ra0");
            if (ra0->getType() == 0) {
                ra0->access_to_value(&nOf);
                for(int i=1; i<nOf+1; i++) {
                    ran = this->find_register_for("ra"+to_string(i));
                    switch(ran->getType()) {
                        case 0 : {
                            ran->access_to_value(&argN);
                            cout << argN;
                        }; break;
                        case 1 : {
                            ran->access_to_value(&argS);
                            cout << argS.substr(1,argS.size()-2);
                        }; break;
                        default: INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE("ra"+to_string(i),"unknown");
                    };
                };
            } else {
                INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE("ra0","numeric");
            };
        };
        
        void query__1() {
            Register * ra0; Register * ran; int nOf,argN; string argS;
            ra0 = this->find_register_for("ra0");
            if (ra0->getType() == 0) {
                ra0->access_to_value(&nOf);
                for(int i=1; i<nOf+1; i++) {
                    ran = this->find_register_for("ra"+to_string(i));
                    
                    switch(ran->getType()) {
                        case 0 : {
                            ran->access_to_value(&argN);
                            switch(argN) {
                                case 0 : {
                                    cin >> argS;
                                    argN = this->get_numeric_value(argS);
                                    getline(cin,argS); // clear cin
                                    ran->write_value(argN);
                                }; break;
                                case 1 : {
                                    getline(cin,argS);
                                    ran->write_value('"'+argS+'"');
                                }; break;
                                default: INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE("ra"+to_string(i),"unknown");
                            };
                        }; break;
                        default: INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE("ra"+to_string(i),"numeric");
                    };
                };
            } else {
                INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE("ra0","numeric");
            };
        };
        
        // query selector
        void query() {
            Register * rid;
            rid = this->find_register_for("rid");
            if (rid->getType() == 0) {
                int ID; rid->access_to_value(&ID);
                switch(ID) {
                    case 0 : {
                        if (PRINT_PROC_LOG)
                            cout << "PROC says, Standard Output." << endl;
                        this->query__0();
                    }; break;
                    
                    case 1 : {
                        if (PRINT_PROC_LOG)
                            cout << "PROC says, Standard Input." << endl;
                        this->query__1();
                    }; break;
                    default: CANNOT_FIND_QUERY(ID);
                };
                
            } else {
                INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE("rid","numeric");
            };
        };
        // tools
        Register * find_register_for(string label) {
            for(auto sec=ptr_to_registers->begin(); sec!=ptr_to_registers->end(); ++sec) {
                if (sec->getLabel() == label) {
                    return &(*sec);
                };
            };
            CANNOT_FIND_REGISTER(label);
            return NULL;
        };
        
        int get_numeric_value(string s) {
            int n;
            try {
                n = stoi(s);
            } catch(...) {
                CANNOT_CONVERT_TO_INT(s);
            };
            return n;
        };
};

// Linker class

class Linker {
    private:
        vector<Section> * ptr_to_sections;
        vector<Register> * ptr_to_registers;
        Processor * ptr_to_processor;
        vector<string> file_linked {};
    public:
        
        Linker() {
            
        };
        
        Linker(vector<Section> * sct, vector<Register> * rgs, Processor * prc) {
            ptr_to_sections = sct;
            ptr_to_registers = rgs;
            ptr_to_processor = prc;
        };
        //
        bool just_linked_this(string filename) {
            for(auto str=file_linked.begin(); str!=file_linked.end(); ++str)
                if ( (*str)==filename )
                    return true;
            return false;
        };
        
        void link(string filename) {
            if (this->just_linked_this(filename))
                return;
            vector<string> vct;
            read_whole_file(filename,&vct);
            file_linked.push_back(filename);
            divide_in_sections(&vct,ptr_to_sections);
            sort_sections(ptr_to_sections);
            merge_equals(ptr_to_sections);
            this->scan_for_links();
        };
        // LINK
        void examine_link_section(Section * link_sect) {
            for(auto inst=link_sect->begin(); inst!=link_sect->end(); ++inst) {
                if (inst->size() == 2) {
                    if (inst->at(0) == "link") {
                        this->link(inst->at(1));
                    };
                };
            };
        };
        
        void scan_for_links() {
            for(auto sec=ptr_to_sections->begin(); sec!=ptr_to_sections->end(); ++sec) {
                if (sec->getLabel() == ".link") {
                    Section S = *sec;
                    ptr_to_sections->erase(sec);
                    this->examine_link_section(&S);
                    sec = ptr_to_sections->begin();
                };
            };
        };
        // DATA
        bool register_is_in_definitions(string label) {
            for(auto r=ptr_to_registers->begin(); r!=ptr_to_registers->end(); ++r) {
                if (r->getLabel() == label)
                    return true;
            };
            return false;
        };
        
        void examine_data_section(Section * data_sect) {
            for(auto inst=data_sect->begin(); inst!=data_sect->end(); ++inst) {
                if (!(this->register_is_in_definitions(inst->at(0)))) {
                    string identifier = inst->at(0);
                    if (inst->size()>1 && inst->at(1)=="db") {
                        if (inst->size()>2 && inst->at(2)[0] == '"') {
                            // all in string
                            string s_ = "";
                            for(long unsigned int i=2; i<inst->size(); i++) {
                                if (inst->at(i)[0] == '"') {
                                    s_ += inst->at(i).substr(1,inst->at(i).size()-2);
                                } else {
                                    s_ += (char)(this->get_numeric_value(inst->at(i)));
                                };
                            };
                            ptr_to_registers->push_back(Register(identifier,'"'+s_+'"',true));
                        } else {
                            // first as number
                            ptr_to_registers->push_back(Register(identifier,this->get_numeric_value(inst->at(2)),true));
                        };
                    } else if (inst->size()>1 && inst->at(1)=="rtb") {
                        ptr_to_registers->push_back(Register(identifier,true));
                    } else {
                        INVALID_SYNTAX_IN_DOT_DATA(&(*inst));
                    };
                };
            };
        };
        
        void scan_for_data() {
            for(auto sec=ptr_to_sections->begin(); sec!=ptr_to_sections->end(); ++sec) {
                if (sec->getLabel() == ".data") {
                    Section S = *sec;
                    ptr_to_sections->erase(sec);
                    this->examine_data_section(&S);
                    --sec;
                };
            };
        };
        //
        void scan_for_path() {
            for(auto sec=ptr_to_sections->begin(); sec!=ptr_to_sections->end(); ++sec) {
                if (sec->getLabel() == ".path") {
                    Section S = *sec;
                    ptr_to_sections->erase(sec);
                    this->run_path(&S);
                    --sec;
                };
            };
        };
        
        Section * scan_for_procedure(string label) {
            for(auto sec=ptr_to_sections->begin(); sec!=ptr_to_sections->end(); ++sec) {
                if (sec->getLabel() == label) {
                    return &(*sec);
                };
            };
            CANNOT_FIND_PROCEDURE(label);
            return NULL;
        };
        
        void run_path(Section * sect) {
            string cmd;
            for(auto inst=sect->begin(); inst!=sect->end(); ++inst) {
                cmd = inst->at(0);
                if (REGISTER_PRINT_MODE)
                    print_registers(ptr_to_registers,35,cmd);
                if (cmd == "mov") {
                    if (inst->size()>=3) {
                        Register * reg = find_register_for(inst->at(1));
                        if (inst->at(2)[0] == '"') {
                            // all in string
                            string s_ = "";
                            for(long unsigned int i=2; i<inst->size(); i++) {
                                if (inst->at(i)[0] == '"') {
                                    s_ += inst->at(i).substr(1,inst->at(i).size()-2);
                                } else {
                                    s_ += (char)(this->get_numeric_value(inst->at(i)));
                                };
                            };
                            reg->write_value('"'+s_+'"');
                        } else {
                            // is a register?
                            if (this->register_is_in_definitions(inst->at(2))) {
                                // yes
                                Register * reg2 = find_register_for(inst->at(2));
                                switch(reg2->getType()) {
                                    case 0 : { int n; reg2->access_to_value(&n); reg->write_value(n); }; break;
                                    case 1 : { string n; reg2->access_to_value(&n); reg->write_value(n); }; break;
                                    default: INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(reg2->getLabel(),"unknown");
                                };
                            } else {
                                // no, so is a number maybe
                                int n = this->get_numeric_value(inst->at(2));
                                reg->write_value(n);
                            };
                        };
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                    
                } else if (cmd == "dec") {
                    if (inst->size()>=3) {
                        if (!(register_is_in_definitions(inst->at(1)))) {
                            ptr_to_registers->push_back(Register(inst->at(1)));
                        } else {
                            ALREADY_DECLARED_REGISTER(inst->at(1));
                        };
                        Register * reg = find_register_for(inst->at(1));
                        reg->scope = sect->getLabel();
                        if (inst->at(2)[0] == '"') {
                            // all in string
                            string s_ = "";
                            for(long unsigned int i=2; i<inst->size(); i++) {
                                if (inst->at(i)[0] == '"') {
                                    s_ += inst->at(i).substr(1,inst->at(i).size()-2);
                                } else {
                                    s_ += (char)(this->get_numeric_value(inst->at(i)));
                                };
                            };
                            reg->write_value('"'+s_+'"');
                        } else {
                            // is a register?
                            if (this->register_is_in_definitions(inst->at(2))) {
                                // yes
                                Register * reg2 = find_register_for(inst->at(2));
                                switch(reg2->getType()) {
                                    case 0 : { int n; reg2->access_to_value(&n); reg->write_value(n); }; break;
                                    case 1 : { string n; reg2->access_to_value(&n); reg->write_value(n); }; break;
                                    default: INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(reg2->getLabel(),"unknown");
                                };
                            } else {
                                // no, so is a number maybe
                                int n = this->get_numeric_value(inst->at(2));
                                reg->write_value(n);
                            };
                        };
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                    
                } else if (cmd == "rsv") {
                    if (inst->size()>=2) {
                        if (!(register_is_in_definitions(inst->at(1)))) {
                            ptr_to_registers->push_back(Register(inst->at(1)));
                        } else {
                            ALREADY_DECLARED_REGISTER(inst->at(1));
                        };
                        Register * reg = find_register_for(inst->at(1));
                        reg->scope = sect->getLabel();
                    };
                } else if (cmd == "drr") {
                    if (inst->size()>=2) {
                        if (!(register_is_in_definitions(inst->at(1)))) {
                            ptr_to_registers->push_back(Register(inst->at(1)));
                        } else {
                            ALREADY_DECLARED_REGISTER(inst->at(1));
                        };
                        Register * reg = find_register_for(inst->at(1));
                        reg->scope = sect->getLabel();
                        reg->invulnerable = true;
                    };
                } else if (cmd == "srr") {
                    if (inst->size()>=2) {
                        Register * reg = find_register_for(inst->at(1));
                        reg->invulnerable = true;
                    };
                } else if (cmd == "add") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                n+=m;
                                
                            } else {
                                // no, so is a number maybe
                                n += this->get_numeric_value(inst->at(i));
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "sub") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                n-=m;
                                
                            } else {
                                // no, so is a number maybe
                                n -= this->get_numeric_value(inst->at(i));
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "mul") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                n*=m;
                                
                            } else {
                                // no, so is a number maybe
                                n *= this->get_numeric_value(inst->at(i));
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "div") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                if (m==0)
                                    INT_DIVISION_BY_ZERO(&(*inst),sect->getLabel());
                                n/=m;
                                
                            } else {
                                // no, so is a number maybe
                                m = this->get_numeric_value(inst->at(i));
                                if (m==0)
                                    INT_DIVISION_BY_ZERO(&(*inst),sect->getLabel());
                                n /= m;
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "mod") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                if (m==0)
                                    INT_DIVISION_BY_ZERO(&(*inst),sect->getLabel());
                                n%=m;
                                
                            } else {
                                // no, so is a number maybe
                                m = this->get_numeric_value(inst->at(i));
                                if (m==0)
                                    INT_DIVISION_BY_ZERO(&(*inst),sect->getLabel());
                                n %= m;
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "xor") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                n^=m;
                                
                            } else {
                                // no, so is a number maybe
                                n ^= this->get_numeric_value(inst->at(i));
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "or") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                n|=m;
                                
                            } else {
                                // no, so is a number maybe
                                n |= this->get_numeric_value(inst->at(i));
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "and") {
                    if (inst->size()>=4) {
                        Register * reg = find_register_for(inst->at(1));
                        int n=0,m;
                        // is this a register?
                        if (this->register_is_in_definitions(inst->at(2))) {
                            // yes
                            Register * reg2;
                            reg2 = find_register_for(inst->at(2));
                            reg2->access_to_value(&m);
                            n+=m;
                        } else {
                            // no, so is a number maybe
                            n += this->get_numeric_value(inst->at(2));
                        };
                        for(long unsigned int i=3; i<inst->size(); ++i) {
                            // is this a register?
                            if (this->register_is_in_definitions(inst->at(i))) {
                                // yes
                                Register * reg2;
                                reg2 = find_register_for(inst->at(i));
                                reg2->access_to_value(&m);
                                n&=m;
                                
                            } else {
                                // no, so is a number maybe
                                n &= this->get_numeric_value(inst->at(i));
                            };
                        }; reg->write_value(n);
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                } else if (cmd == "del") {
                    if (inst->size() < 2)
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    for(long unsigned int i=1; i<inst->size(); i++) {
                        bool found=false;
                        for(auto reg=ptr_to_registers->begin(); reg!=ptr_to_registers->end(); ++reg) {
                            if (reg->getLabel() == inst->at(i)) {
                                ptr_to_registers->erase(reg);
                                found = true;
                                break;
                            };
                        };
                        if (!found)
                            CANNOT_FIND_REGISTER(inst->at(i));
                    };
                } else if (cmd == "rst") {
                    if (inst->size() < 2)
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    for(long unsigned int i=1; i<inst->size(); i++) {
                        bool found=false;
                        for(auto reg=ptr_to_registers->begin(); reg!=ptr_to_registers->end(); ++reg) {
                            if (reg->getLabel() == inst->at(i)) {
                                reg->reset();
                                found = true;
                                break;
                            };
                        };
                        if (!found)
                            CANNOT_FIND_REGISTER(inst->at(i));
                    };
                } else if (cmd == "run") {
                    if (inst->size() < 2)
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    for(long unsigned int i=2; i<inst->size(); i++) {
                        bool found=false;
                        for(auto reg=ptr_to_registers->begin(); reg!=ptr_to_registers->end(); ++reg) {
                            if (reg->getLabel() == inst->at(i)) {
                                *(find_register_for("ra"+to_string(i-2))) = &(*reg);
                                found = true;
                                break;
                            };
                        };
                        if (!found)
                            CANNOT_FIND_REGISTER(inst->at(i));
                    };
                    this->run_path(scan_for_procedure(inst->at(1)));
                } else if (cmd == "pik") {
                    if (inst->size() < 3)
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    for(long unsigned int i=3; i<inst->size(); i++) {
                        bool found=false;
                        for(auto reg=ptr_to_registers->begin(); reg!=ptr_to_registers->end(); ++reg) {
                            if (reg->getLabel() == inst->at(i)) {
                                *(find_register_for("ra"+to_string(i-3))) = &(*reg);
                                found = true;
                                break;
                            };
                        };
                        if (!found)
                            CANNOT_FIND_REGISTER(inst->at(i));
                    };
                    this->run_path(scan_for_procedure(inst->at(2)));
                    Register * reg = find_register_for(inst->at(1));
                    *reg = find_register_for("rrr");
                } else if (cmd == "ret") {
                    if (inst->size()>=2) {
                        Register * reg = find_register_for("rrr");
                        if (inst->at(1)[0] == '"') {
                            // all in string
                            string s_ = "";
                            for(long unsigned int i=1; i<inst->size(); i++) {
                                if (inst->at(i)[0] == '"') {
                                    s_ += inst->at(i).substr(1,inst->at(i).size()-2);
                                } else {
                                    s_ += (char)(this->get_numeric_value(inst->at(i)));
                                };
                            };
                            reg->write_value('"'+s_+'"');
                        } else {
                            // is a register?
                            if (this->register_is_in_definitions(inst->at(1))) {
                                // yes
                                Register * reg2 = find_register_for(inst->at(1));
                                switch(reg2->getType()) {
                                    case 0 : { int n; reg2->access_to_value(&n); reg->write_value(n); }; break;
                                    case 1 : { string n; reg2->access_to_value(&n); reg->write_value(n); }; break;
                                    default: INVALID_ACCESS_TO_REGISTER_HOLDING_OTHER_TYPE_OF_VALUE(reg2->getLabel(),"unknown");
                                };
                            } else {
                                // no, so is a number maybe
                                int n = this->get_numeric_value(inst->at(1));
                                reg->write_value(n);
                            };
                        }; break;
                    } else {
                        INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                    };
                    
                } else if (cmd == "proc") {
                    ptr_to_processor->query();
                } else if (cmd == "exit") {
                    break;
                } else if (cmd == "quit") {
                    if (START_AND_STOP_MODE)
                        cout << "end : " << ptr_to_registers->size() << " final regs" << endl;
                    if (REGISTER_PRINT_MODE)
                        print_registers(ptr_to_registers);
                    exit(0);
                } else {
                    INVALID_SYNTAX_IN_PROCEDURE(&(*inst),sect->getLabel());
                };
            };
            this->clean_registers(sect->getLabel());
        };
        
        Register * find_register_for(string label) {
            for(auto sec=ptr_to_registers->begin(); sec!=ptr_to_registers->end(); ++sec) {
                if (sec->getLabel() == label) {
                    return &(*sec);
                };
            };
            CANNOT_FIND_REGISTER(label);
            return NULL;
        };
        
        int get_numeric_value(string s) {
            int n;
            try {
                n = stoi(s);
            } catch(...) {
                CANNOT_CONVERT_TO_INT(s);
            };
            return n;
        };
        // clean
        void clean_registers(string label) {
            for(auto reg=ptr_to_registers->begin(); reg!=ptr_to_registers->end(); ++reg) {
                if ((reg->scope == label) && (reg->invulnerable==false)) {
                    ptr_to_registers->erase(reg);
                    --reg;
                };
            };
        };
};

// static versions
int get_numeric_value(string s) {
    int n;
    try {
        n = stoi(s);
    } catch(...) {
        CANNOT_CONVERT_TO_INT(s);
    };
    return n;
};

// globals

vector<Register> registers {};
vector<Section> sections {};
vector<string> sources_passed {};
Processor processor(&registers);
Linker linker (&sections,&registers,&processor);

// generate system registers
void generate_system_registers() {
    registers.push_back(Register("rid",true));
    for(int i=0; i<NUMBER_OF_SYSTEM_REGISTERS; i++) {
        registers.push_back(Register("ra"+to_string(i),true));
    };
};

void generate_return_registers() {
    registers.push_back(Register("rrr",true));
};

// option parse

void analize_option(string opt) {
    if (opt == "spm") {
        SECTION_PRINT_MODE = true;
    } else if (opt == "rpm") {
        REGISTER_PRINT_MODE = true;
    } else if (opt == "sasm") {
        START_AND_STOP_MODE = true;
    } else if (opt == "nsr") {
        NO_SYSTEM_REGISTERS = true;
    } else if (opt == "nrr") {
        NO_RETURN_REGISTERS = true;
    } else if (opt == "pci") {
        PRINT_CMDLINE_INPUTS = true;
    } else if (opt == "ppl") {
        PRINT_PROC_LOG = true;
    } else {
        OPTION_NOT_RECOGNIZED(opt);
    };
};

int find_char_in_string_from_bottom(string s, char c) {
    int j=0;
    for(int i=s.size()-1; i>-1; i--,j++) {
        if (s[i] == c)
            return j;
    };
    return -1;
};

string extract_ext_from_source_name(string s, int k) {
    return s.substr(s.size()-k,k);
};

void verify_ext_of_source_name(string s) {
    int k = find_char_in_string_from_bottom(s,'.');
    if (k == -1)
        MISSING_EXT_ERROR(s);
    string ext = extract_ext_from_source_name(s,k);
    if (!(ext == "ref"))
        INVALID_EXT_ERROR("."+ext);
};

void analize_passed_sources(int argc, char**args) {
    for(int i=1; i<argc; i++) {
        string line = args[i];
        if (line.size()<2 || line.substr(0,2)!="--") {
            verify_ext_of_source_name(line);
            sources_passed.push_back(line);
        } else {
            if (line.substr(2,line.size()-2) == "nosr") {
                if (argc-i>=2)
                    NUMBER_OF_SYSTEM_REGISTERS = get_numeric_value(string(args[i+1]));
                else
                    EXPECTED_ARGUMENT_AFTER_OPTION(line.substr(2,line.size()-2));
            } else {
                analize_option(line.substr(2,line.size()-2));
            };
        };
    };
};

void print_main_args(int argc,char**args) {
    cout << "cmdline inputs : {";
    for(int i=0; i<argc; i++) {
        cout << "\t" << args[i] << endl;
    };
    cout << "}" << endl;
};

// main

int main(int argc, char**args) {
    if (PRINT_CMDLINE_INPUTS)
        print_main_args(argc,args);
    analize_passed_sources(argc, args);
    if (sources_passed.size() == 0)
        NO_SOURCE_PASSED();
    for (string source : sources_passed)
        linker.link(source);
    if (SECTION_PRINT_MODE)
        print_sections(&sections);
    linker.scan_for_data();
    if (START_AND_STOP_MODE)
        cout << "ready : " << registers.size() << " initial regs" << endl;
    if (SECTION_PRINT_MODE)
        print_sections(&sections);
    if (!NO_SYSTEM_REGISTERS)
        generate_system_registers();
    if (!NO_RETURN_REGISTERS)
        generate_return_registers();
    linker.scan_for_path();
    if (REGISTER_PRINT_MODE)
        print_registers(&registers);
    if (START_AND_STOP_MODE)
        cout << "end : " << registers.size() << " final regs" << endl;
};
