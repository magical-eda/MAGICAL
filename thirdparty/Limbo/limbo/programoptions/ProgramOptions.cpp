/**
 @file   ProgramOptions.cpp
 @author Yibo Lin
 @date   Jul 2015
 @brief  Implementation of @ref limbo::programoptions::ProgramOptions
 */

#include "ProgramOptions.h"

namespace limbo { namespace programoptions {

ProgramOptions::~ProgramOptions()
{
    for (std::vector<ValueBase*>::iterator it = m_vData.begin(); it != m_vData.end(); ++it)
        delete *it;
}

bool ProgramOptions::parse(int argc, char** argv)
{
    // skip the first argument 
    for (int i = 1; i < argc; ++i)
    {
#ifdef DEBUG
        printf("argv[%d] = %s\n", i, argv[i]);
#endif
        cat2index_map_type::iterator found = m_mCat2Index.find(argv[i]);
        if (found != m_mCat2Index.end())
        {
            ValueBase* pData = m_vData.at(found->second);
            if (pData->toggle()) // toggle option 
                pData->apply_toggle();
            else // non-toggle option 
            {
                i += 1;
                if (i < argc) 
                {
                    const char* value = argv[i];
                    bool flag = pData->parse(value);
                    if (!flag) // fail or help 
                        throw ProgramOptionsException(std::string("failed to parse ")+value);
                }
                else 
                    throw ProgramOptionsException("invalid appending option");
            }
        }
        else 
            throw ProgramOptionsException(std::string("unknown option: ")+argv[i]);
    }
    // check required and default value 
    std::ostringstream oss;
    bool help = false;
    const char* prefix = "";
    for (cat2index_map_type::const_iterator it = m_mCat2Index.begin(); it != m_mCat2Index.end(); ++it)
    {
        std::string const& category = it->first;
        ValueBase* pData = m_vData.at(it->second);
        if (!pData->valid()) // not set 
        {
            if (pData->valid_default()) // has default 
                pData->apply_default();
            else if (pData->required()) // required 
            {
                oss << prefix << category;
                prefix = ", ";
            }
        }
        if (pData->help_on()) // help is true 
            help = true;
    }
    // if help is on, no need to set other required options
    if (!help && !oss.str().empty())
        throw ProgramOptionsException(std::string("required option not set: ")+oss.str());
    return true;
}

bool ProgramOptions::count(std::string const& cat) const 
{
    cat2index_map_type::const_iterator found = m_mCat2Index.find(cat);
    if (found != m_mCat2Index.end())
        return m_vData.at(found->second)->valid();
    else return false;
}

void ProgramOptions::print(std::ostream& os) const 
{
    static const unsigned cat_prefix_spaces = 2;
    static const unsigned cat_option_spaces = 4;

    unsigned max_cat_count = 0;
    std::vector<unsigned> vPlaceholderLength;
    vPlaceholderLength.reserve(m_vData.size());

    os << m_title << ":" << std::endl;

    // calculate length of placeholders
    for (std::vector<ValueBase*>::const_iterator it = m_vData.begin(); it != m_vData.end(); ++it)
    {
        const ValueBase* pData = *it;
        // category (default value)
        unsigned extra_chars = 0;
        if (pData->valid_default()) // extra 3 characters 
            extra_chars = 3;
        vPlaceholderLength.push_back(pData->category().size()+pData->count_default_chars()+extra_chars);
        max_cat_count = std::max(max_cat_count, vPlaceholderLength.back()); 
    }
    std::vector<unsigned>::const_iterator itPhl = vPlaceholderLength.begin();
    for (std::vector<ValueBase*>::const_iterator it = m_vData.begin(); it != m_vData.end(); ++it, ++itPhl)
    {
        const ValueBase* pData = *it;
        // print cat 
        print_space(os, cat_prefix_spaces);
        pData->print_category(os);
        // print default value 
        if (pData->valid_default())
        {
            os << " ("; 
            pData->print_default(os);
            os << ")";
        }
        // print message 
        print_space(os, max_cat_count-*itPhl+cat_option_spaces);
        pData->print_msg(os);
        os << "\n";
    }
}

}} // namespace limbo // programoptions
