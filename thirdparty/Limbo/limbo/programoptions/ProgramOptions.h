/**
 * @file   ProgramOptions.h
 * @brief  Top API for Limbo.ProgramOptions
 *
 * Mimic Boost.ProgramOptions with easy wrapper for program to parser command line options
 *
 * @author Yibo Lin
 * @date   Jul 2015
 */

#ifndef _LIMBO_PROGRAMOPTIONS_PROGRAMOPTIONS_H
#define _LIMBO_PROGRAMOPTIONS_PROGRAMOPTIONS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <limbo/string/String.h>
#include <limbo/preprocessor/AssertMsg.h>
#include <limbo/programoptions/ConversionHelpers.h>

/// @brief namespace for Limbo
namespace limbo 
{ 
/// @brief namespace for Limbo.ProgramOptions
namespace programoptions 
{

/**
 * @class limbo::programoptions::ProgramOptionsException
 * @brief an exception class for @ref limbo::programoptions::ProgramOptions
 */
class ProgramOptionsException : public std::exception
{
    public:
        /// base type 
        typedef std::exception base_type;
        /**
         * @brief constructor
         * @param msg message 
         */
        ProgramOptionsException(std::string const& msg) : base_type(), m_msg(msg) {}
        /**
         * @brief copy constructor
         * @param rhs the other object to copy 
         */
        ProgramOptionsException(ProgramOptionsException const& rhs) : base_type(rhs), m_msg(rhs.m_msg) {}
        /**
         * @brief destructor 
         */
        virtual ~ProgramOptionsException() throw() {}
        /**
         * @brief access message 
         * @return message of the exception
         */
        virtual const char* what() const throw () {return m_msg.c_str();}
    protected:
        std::string m_msg; ///< message of the exception
};

/**
 * @class limbo::programoptions::ValueBase
 * @brief abstract type of data value which defines various virtual functions 
 */
class ValueBase
{
    public:
        /**
         * @brief constructor
         * @param cat category
         * @param m message
         */
        ValueBase(std::string const& cat, std::string const& m)
            : m_category(cat)
            , m_msg(m)
            , m_help(false)
            , m_required(false)
            , m_valid(false)
            , m_toggle(false)
        {}
        /**
         * @brief copy constructor
         * @param rhs the other object to copy
         */
        ValueBase(ValueBase const& rhs) {copy(rhs);}
        /**
         * @brief assignment
         * @param rhs the other object to copy
         * @return reference to current object
         */
        ValueBase& operator=(ValueBase const& rhs)
        {
            if (this != &rhs)
                copy(rhs);
            return *this;
        }
        /**
         * @brief destructor
         */
        virtual ~ValueBase() {}

        /// parse command 
        /**
         * @brief pure virtual function to parse string 
         * @return true if succeeded 
         */
        virtual bool parse(const char*) = 0;
        /**
         * @brief print target value 
         * @param os output stream 
         */
        virtual void print(std::ostream& os) const = 0;
        /**
         * @brief print default value 
         * @param os output stream 
         */
        virtual void print_default(std::ostream& os) const = 0;
        /**
         * @brief apply default value 
         */
        virtual void apply_default() = 0;
        /**
         * @brief apply toggle value 
         */
        virtual void apply_toggle() = 0;
        /**
         * @brief check whether target value is valid 
         * @return true if target pointer is defined 
         */
        virtual bool valid_target() const = 0;
        /**
         * @brief check whether default value is valid 
         * @return true if default value is set 
         */
        virtual bool valid_default() const = 0;
        /**
         * @brief check whether toggle value is valid 
         * @return true if toggle value is set 
         */
        virtual bool valid_toggle() const = 0;
        /**
         * @brief count the length of default value string
         * @return the length of string if default value is printed 
         */
        virtual unsigned count_default_chars() const = 0;
        /**
         * @brief check whether help message is turned on 
         * @return true if this option is a help option and it is on 
         */
        virtual bool help_on() const = 0;

        /**
         * @return category
         */
        std::string const& category() const {return m_category;}
        /**
         * @return message
         */
        std::string const& msg() const {return m_msg;}
        /**
         * @return help flag
         */
        bool help() const {return m_help;}
        /**
         * @return required flag 
         */
        bool required() const {return m_required;}
        /**
         * @return category
         */
        bool valid() const {return m_valid;}
        /**
         * @return toggle flag
         */
        bool toggle() const {return m_toggle;}
        /**
         * @brief print category
         * @param os output stream
         */
        void print_category(std::ostream& os) const {os << category();}
        /**
         * @brief print message
         * @param os output stream
         */
        void print_msg(std::ostream& os) const {os << msg();}
    protected:
        /**
         * @brief copy another object 
         * @param rhs another object 
         */
        void copy(ValueBase const& rhs)
        {
            m_category = rhs.m_category;
            m_msg = rhs.m_msg;
            m_help = rhs.m_help;
            m_required = rhs.m_required;
            m_valid = rhs.m_valid;
            m_toggle = rhs.m_toggle;
        }

        std::string m_category; ///< category 
        std::string m_msg; ///< helper message 
        unsigned char m_help : 1; ///< whether is help option 
        unsigned char m_required : 1; ///< whether the value is a required option
        unsigned char m_valid : 1; ///< true if target is set, not default 
        unsigned char m_toggle : 1; ///< true if this option is a toggle value 
};

/**
 * @class limbo::programoptions::Value
 * @brief a generic class for various data values 
 * @tparam T data type 
 */
template <typename T>
class Value : public ValueBase
{
    public:
        /// data type 
        typedef T value_type;
        /// base type 
        typedef ValueBase base_type;

        /**
         * @brief constructor
         * @param cat category
         * @param target a pointer of target 
         * @param m message 
         */
        Value(std::string const& cat, value_type* target, std::string const& m)
            : base_type(cat, m)
            , m_target(target)
            , m_default_value(NULL)
            , m_toggle_value(NULL)
            , m_default_display("")
        {}
        /**
         * @brief copy constructor
         * @param rhs the other object 
         */
        Value(Value const& rhs) 
            : base_type(rhs)
        {
            copy(rhs);
        }
        /**
         * @brief assignment
         * @param rhs the other object 
         * @return reference to the object 
         */
        Value& operator=(Value const& rhs)
        {
            if (this != &rhs)
            {
                this->base_type::copy(rhs);
                this->copy(rhs);
            }
            return *this;
        }
        /**
         * @brief destructor
         */
        virtual ~Value() 
        {
            if (m_default_value)
                delete m_default_value;
            if (m_toggle_value)
                delete m_toggle_value;
        }

        /**
         * @brief parse data from string 
         * @param v data value in string  
         * @return true if parsing is succeeded
         */
        virtual bool parse(const char* v) 
        {
            if (m_target)
            {
                bool flag = parse_helper<value_type>()(*m_target, v);
                if (flag) 
                    m_valid = true;
                return flag;
            }
            return false;
        }
        /**
         * @brief print target value 
         * @param os output stream 
         */
        virtual void print(std::ostream& os) const 
        {
            if (m_target)
                print_helper<value_type>()(os, *m_target);
        }
        /**
         * @brief print default value 
         * @param os output stream
         */
        virtual void print_default(std::ostream& os) const 
        {
            if (m_default_value)
            {
                if (m_default_display.empty())
                    print_helper<value_type>()(os, *m_default_value);
                else os << m_default_display;
            }
        }
        /**
         * @brief apply default value
         */
        virtual void apply_default()
        {
            if (m_target && m_default_value)
                assign_helper<value_type>()(*m_target, *m_default_value);
        }
        /**
         * @brief apply toggle value
         */
        virtual void apply_toggle()
        {
            if (m_target && m_toggle_value)
            {
                assign_helper<value_type>()(*m_target, *m_toggle_value);
                m_valid = true;
            }
        }
        /**
         * @brief check whether target value is valid 
         * @return true if target pointer is defined 
         */
        virtual bool valid_target() const
        {
            return (m_target != NULL);
        }
        /**
         * @brief check whether default value is valid 
         * @return true if default pointer is defined 
         */
        virtual bool valid_default() const 
        {
            return (m_default_value != NULL);
        }
        /**
         * @brief check whether toggle value is valid 
         * @return true if toggle pointer is defined 
         */
        virtual bool valid_toggle() const 
        {
            return (m_toggle_value != NULL);
        }
        /**
         * @brief count the length of default value string
         * @return the length of string if default value is printed 
         */
        virtual unsigned count_default_chars() const 
        {
            std::ostringstream oss;
            print_default(oss);
            return oss.str().size();
        }
        /**
         * @brief check whether help message is turned on 
         * @return true if this option is a help option and it is on 
         */
        virtual bool help_on() const 
        {
            // only true when this option is already set 
            if (m_help && m_target && boolean_helper<value_type>()(*m_target))
                return true;
            else return false;
        }
        /**
         * @brief set default value 
         * @param v value 
         * @param d a string to display the value 
         * @return reference to the object
         */
        virtual Value& default_value(value_type const& v, std::string const& d = "")
        {
            if (m_default_value) // in case for multiple calls 
                delete m_default_value;
            m_default_value = new value_type (v);
            m_default_display = d;
            return *this;
        }
        /**
         * @brief set toggle value 
         * @param v value 
         * @return reference to the object
         */
        virtual Value& toggle_value(value_type const& v)
        {
            if (m_toggle_value) // in case for multiple calls 
                delete m_toggle_value;
            m_toggle_value = new value_type (v);
            return *this;
        }
        /**
         * @brief set help flag 
         * @param h value 
         * @return reference to the object
         */
        virtual Value& help(bool h)
        {
            m_help = h;
            return *this;
        }
        /**
         * @brief set required flag 
         * @param r value 
         * @return reference to the object
         */
        virtual Value& required(bool r)
        {
            m_required = r;
            return *this;
        }
        /**
         * @brief set toggle flag 
         * @param t value 
         * @return reference to the object
         */
        virtual Value& toggle(bool t)
        {
            m_toggle = t;
            return *this;
        }

    protected:
        /**
         * @brief copy another object 
         * @param rhs another object 
         */
        void copy(Value const& rhs)
        {
            m_target = rhs.m_target;
            m_default_value = NULL;
            m_toggle_value = NULL;
            if (rhs.m_default_value)
                default_value(*rhs.m_default_value, rhs.m_default_display);
            if (rhs.m_toggle_value)
                toggle_value(*rhs.m_toggle_value);
        }

        value_type* m_target; ///< NULL for help 
        value_type* m_default_value; ///< default value 
        value_type* m_toggle_value; ///< only valid when this option is a toggle value 
        std::string m_default_display; ///< display default value 
};

/**
 * @class limbo::programoptions::ProgramOptions
 * @brief top API to parse program options
 */
class ProgramOptions
{
    public:
        /// mapping from category to index 
        typedef std::map<std::string, unsigned> cat2index_map_type;

        /**
         * @brief constructor
         * @param title title to display in help message 
         */
        ProgramOptions(std::string const& title = "Available options");
        /**
         * @brief copy constructor
         * @param rhs the other object
         */
        ProgramOptions(ProgramOptions const& rhs);
        /**
         * @brief destructor
         */
        ~ProgramOptions();

        /**
         * @brief generic API to add options of various data types 
         * @tparam ValueType data type 
         * @param data except instantiations of @ref limbo::programoptions::Value
         */
        template <typename ValueType>
        ProgramOptions& add_option(ValueType const& data);
        /**
         * @brief read command line options 
         * @param argc number of options 
         * @param argv values of options
         */
        bool parse(int argc, char** argv);

        /**
         * @return true if the option is set by command 
         */
        bool count(std::string const& cat) const; 

        /**
         * @brief print help message 
         */
        void print() const {print(std::cout);}
        /**
         * @brief print help message 
         * @param os output stream
         */
        void print(std::ostream& os) const;
        /**
         * @brief print help message by override operator<< 
         * @param os output stream
         * @param rhs the object 
         * @return reference to os
         */
        friend std::ostream& operator<<(std::ostream& os, ProgramOptions const& rhs)
        {
            rhs.print(os);
            return os;
        }
    protected:
        /**
         * @brief print a specific number of spaces 
         */
        inline void print_space(std::ostream& os, unsigned num) const 
        {
            assert_msg(num < 1000, "num out of bounds: " << num);
            os << std::string (num, ' ');
        }

        std::map<std::string, unsigned> m_mCat2Index; ///< saving mapping for flag to option
        std::vector<ValueBase*> m_vData; ///< saving options 
        std::string m_title; ///< title of options 
};

inline ProgramOptions::ProgramOptions(std::string const& title) 
    : m_title(title) 
{}

template <typename ValueType>
ProgramOptions& ProgramOptions::add_option(ValueType const& data) 
{
    std::pair<cat2index_map_type::iterator, bool> insertRet = m_mCat2Index.insert(std::make_pair(data.category(), m_vData.size()));
    if (insertRet.second) // only create new data when it is not in the map
        m_vData.push_back(new ValueType (data));
    return *this;
}

} // programoptions
} // namespace limbo

#endif
