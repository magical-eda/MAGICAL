/**
 * @file   gdsii/test_reader.cpp
 * @brief  test @ref GdsParser::GdsReader 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#include <iostream>
#include <limbo/parsers/gdsii/stream/GdsReader.h>
using std::cout;
using std::endl;

/// @brief test ascii callbacks 
struct AsciiDataBase : public GdsParser::GdsDataBase
{
    /// @brief constructor 
	AsciiDataBase()
	{
		cout << "constructing AsciiDataBase" << endl;
	}
	///////////////////// required callbacks /////////////////////
    /// @brief bit array callback 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param vBitArray data array  
	virtual void bit_array_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vBitArray)
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, ascii_data_type, vBitArray);
	}
    /// @brief 2-byte integer callback 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param vInteger data array  
	virtual void integer_2_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger)
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, ascii_data_type, vInteger);
	}
    /// @brief 4-byte integer callback 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param vInteger data array  
	virtual void integer_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger)
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, ascii_data_type, vInteger);
	}
    /// @brief 4-byte floating point number callback 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param vFloat data array  
	virtual void real_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) 
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, ascii_data_type, vFloat);
	}
    /// @brief 8-byte floating point number callback 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param vFloat data array  
	virtual void real_8_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) 
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, ascii_data_type, vFloat);
	}
    /// @brief string callback 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param str data 
	virtual void string_cbk(const char* ascii_record_type, const char* ascii_data_type, string const& str) 
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, ascii_data_type, str);
	}
    /// @brief begin or end indicator of a block 
    /// @param ascii_record_type record 
	virtual void begin_end_cbk(const char* ascii_record_type)
	{
		cout << __func__ << endl;
		this->general_cbk(ascii_record_type, "", vector<int>(0));
	}

    /// @brief A generic callback function handles all other callback functions. 
    /// It is not efficient but concise as a demo. 
    /// @tparam ContainerType container type 
    /// @param ascii_record_type record 
    /// @param ascii_data_type data type 
    /// @param data data values 
	template <typename ContainerType>
	void general_cbk(string const& ascii_record_type, string const& ascii_data_type, ContainerType const& data)
	{
		cout << "ascii_record_type: " << ascii_record_type << endl
			<< "ascii_data_type: " << ascii_data_type << endl 
			<< "data size: " << data.size() << endl;
		if (ascii_record_type == "UNITS")
		{
		}
		else if (ascii_record_type == "BOUNDARY")
		{
		}
		else if (ascii_record_type == "LAYER")
		{
		}
		else if (ascii_record_type == "XY")
		{
			cout << data.size() << endl;
		}
		else if (ascii_record_type == "ENDEL")
		{
		}
	}
};

/// @brief test enum callbacks
struct EnumDataBase : public GdsParser::GdsDataBaseKernel
{
    /// @brief constructor 
    EnumDataBase()
    {
        cout << "constructing EnumDataBase" << endl;
    }
    ///////////////////// required callbacks /////////////////////
    /// @brief bit array callback 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param vBitArray data array  
    virtual void bit_array_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, vector<int> const& vBitArray)
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, data_type, vBitArray);
    }
    /// @brief 2-byte integer callback 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param vInteger data array  
    virtual void integer_2_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, vector<int> const& vInteger)
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, data_type, vInteger);
    }
    /// @brief 4-byte integer callback 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param vInteger data array  
    virtual void integer_4_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, vector<int> const& vInteger)
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, data_type, vInteger);
    }
    /// @brief 4-byte floating point number callback 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param vFloat data array  
    virtual void real_4_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, vector<double> const& vFloat) 
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, data_type, vFloat);
    }
    /// @brief 8-byte floating point number callback 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param vFloat data array  
    virtual void real_8_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, vector<double> const& vFloat) 
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, data_type, vFloat);
    }
    /// @brief string callback 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param str data 
    virtual void string_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, string const& str) 
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, data_type, str);
    }
    /// @brief begin or end indicator of a block 
    /// @param record_type record 
    virtual void begin_end_cbk(GdsParser::GdsRecords::EnumType record_type)
    {
        cout << __func__ << endl;
        this->general_cbk(record_type, GdsParser::GdsData::NO_DATA, vector<int>(0));
    }

    /// @brief A generic callback function handles all other callback functions. 
    /// It is not efficient but concise as a demo. 
    /// @tparam ContainerType container type 
    /// @param record_type record 
    /// @param data_type data type 
    /// @param data data values 
    template <typename ContainerType>
    void general_cbk(GdsParser::GdsRecords::EnumType record_type, GdsParser::GdsData::EnumType data_type, ContainerType const& data)
    {
        cout << "ascii_record_type: " << GdsParser::gds_record_ascii(record_type) << endl
            << "ascii_data_type: " << GdsParser::gds_data_ascii(data_type) << endl 
            << "data size: " << data.size() << endl;
        switch (record_type)
        {
            case GdsParser::GdsRecords::UNITS:
                break;
            case GdsParser::GdsRecords::BOUNDARY:
                break;
            case GdsParser::GdsRecords::LAYER:
                cout << "LAYER = " << data[0] <<  endl;
                break;
            case GdsParser::GdsRecords::XY:
                for (typename ContainerType::const_iterator it = data.begin(); it != data.end(); ++it)
                    cout << *it << " "; 
                cout << endl; 
                cout << data.size() << endl;
                break;
            case GdsParser::GdsRecords::ENDEL:
                break;
            default:
                break;
        }
    }
};

/* ===========================================
example to read .gds.gz 
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>

EnumDataBase edb; 
boost::iostreams::filtering_istream in; 
in.push(boost::iostreams::gzip_decompressor());
in.push(boost::iostreams::file_source(argv[1]));

cout << "test enum api\n" << GdsParser::read(edb, in) << endl;
=========================================== */

/// @brief main function 
/// @param argc number of arguments 
/// @param argv values of arguments 
/// @return 0 if succeed 
int main(int argc, char** argv)
{
	if (argc > 1)
    {
        AsciiDataBase adb;
		cout << "test ascii api\n" << GdsParser::read(adb, argv[1]) << endl;
        EnumDataBase edb;
		cout << "test enum api\n" << GdsParser::read(edb, argv[1]) << endl;
    }
	else cout << "at least 1 argument is required" << endl;

	return 0;
}
