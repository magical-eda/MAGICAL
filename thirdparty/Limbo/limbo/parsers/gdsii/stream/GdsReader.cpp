/**
 * @file   GdsReader.cpp
 * @author Jim Buchanan, Yibo Lin
 * @brief  GDSII reader
 *
 * It is originally developed by Jim Buchanan and released under GPL. 
 *
 * sf2ascii
 * 
 * You can contact me via email at:
 * 
 * jim@buchanan1.net
 *
 * Copyright (C) 1996-1998 Jim Buchanan
 *
 * This program may be freely distributed under the terms of the GPL
 * General Public License, any version you prefer. The GPL may be found
 * at http://www.gnu.org/
 *
 */

/* I tried to make this really robust. In theory, no matter how corrupt the file is, this should
 * completely read it and output it, in raw form if nothing else. In theory, it should
 * be possible to run any binary file through here w/o a crash. ascii2sf is
 * not guaranteed to get it back though, since floating point numbers may not
 * survive the double translation w/o minor changes way over to the right of
 * the decimal */

/* Modified by Yibo Lin 
 * Now it is able to be integrated into C++ applications
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <fstream>
#include <limbo/parsers/gdsii/stream/GdsReader.h>
/// support to .gds.gz if BOOST is available 
/// better to put them in .cpp, which is not seen by users 
#if ZLIB == 1 
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#endif

/* this is how far we indent the structures, then the elements */
#define NO_SPACES_TO_INDENT 2

namespace GdsParser
{

/* static function */
bool read(GdsDataBaseKernel& db, std::istream& fp)
{
	return GdsReader(db)(fp);
}

bool read(GdsDataBaseKernel& db, string const& filename)
{
/// support to .gds.gz if BOOST is available 
#if ZLIB == 1
    if (limbo::get_file_suffix(filename) == "gz") // detect .gz file 
    {
        boost::iostreams::filtering_istream in; 
        in.push(boost::iostreams::gzip_decompressor());
        in.push(boost::iostreams::file_source(filename.c_str()));
        return read(db, in);
    }
#endif
    return GdsReader(db)(filename.c_str());
}

GdsReader::GdsReader(GdsDataBaseKernel& db) 
    : m_db(db) 
{
    m_bcap = 4*1024; // 4 KB
    m_blen = 0; 
    m_buffer = new char [m_bcap]; 
    m_bptr = m_buffer; 
}

GdsReader::~GdsReader()
{
    delete [] m_buffer; 
    m_bptr = NULL; 
}

/// copy char* as unsigned long* and deal with boundary cases 
inline void fast_copy (char *t, const char *s, std::size_t n)
{
    if (n >= sizeof (unsigned long)) 
    {
        unsigned long *tl = reinterpret_cast<unsigned long *> (t);
        const unsigned long *sl = reinterpret_cast<const unsigned long *> (s);

        while (n >= sizeof (unsigned long)) 
        {
            *tl++ = *sl++;
            n -= sizeof (unsigned long);
        }

        t = reinterpret_cast<char *> (tl);
        s = reinterpret_cast<const char *> (sl);
    }

    while (n-- > 0) 
    {
        *t++ = *s++;
    }
}

const char* GdsReader::gds_read(std::istream& fp, int& no_read, std::size_t n)
{
    if (m_blen < n) // content in the buffer is not enough 
    {
        //  to keep move activity low, allocate twice as much as required
        if (m_bcap < n * 2) 
        {
            while (m_bcap < n) // amortize doubling 
            {
                m_bcap *= 2;
            }

            char *buffer = new char [m_bcap];
            if (m_blen > 0) 
            {
                // it seems memcpy is faster than fast_copy here 
                memcpy (buffer, m_bptr, m_blen);
                //fast_copy(buffer, m_bptr, m_blen); 
            }
            delete [] m_buffer;
            m_buffer = buffer;

        } 
        else if (m_blen > 0) // move existing content front 
        {
            memmove (m_buffer, m_bptr, m_blen);
        }

        fp.read(m_buffer + m_blen, m_bcap - m_blen);
        int num_bytes = fp.gcount(); 
        if (num_bytes < 0) // error 
        {
            no_read = 0; 
            return NULL; 
        }
        m_blen += num_bytes;
        m_bptr = m_buffer;
    }

    if (m_blen >= n) 
    {
        const char *r = m_bptr;
        m_bptr += n;
        m_blen -= n;
        no_read = n; 
        return r;
    } 
    else 
    {
        no_read = 0; 
        return NULL;
    }
}

bool GdsReader::operator() (const char* filename)
{
    std::ifstream fp (filename);
    if (!fp.good())
    {
		printf("failed to open %s for read\n", filename);
		return false;
    }

    bool flag = (*this)(fp);
    fp.close();
    return flag; 
}

bool GdsReader::operator() (std::istream& fp)
{
    // unsigned char no_byte_array[2]; 
	unsigned char* no_byte_array;
	int no_read;
	int no_bytes;
	unsigned char* record;
    int indent_amount;
	int record_type;
	int data_type;
	int expected_data_type;
    GdsRecords::EnumType enum_record_type;
    GdsData::EnumType enum_data_type;
    GdsData::EnumType enum_expected_data_type;
	int int_ktr;
	int data_ktr;
	int exponent_ktr;
	int corrupt_ktr;
	unsigned int display_integer;
#ifdef DEBUG_GDSREADER
	unsigned int hex_display_integer;
#endif 
	char display_char_1;
	char display_char_2;
	int bit_array;
	int real_sign;
	int real_exponent;
	unsigned long long real_mantissa_int;	/* 64 bit integer, yep you need
											 * at least 56 of 'em. Some compilers need
											 * a switch to allow this. Maybe some don't.
											 * gcc has no problems... */
	double real_mantissa_float;
	double display_float;

	/* start out with no indent */
    indent_amount = 0;

	/* main loop, read the record header, then read the rest of the record once we know its
	 * size */
	while (1)
	{
		/* read 2 bytes, this will be two bytes telling how many bytes of
		 * data are in this record (including these two bytes) */
		//no_read = ::read (fp, no_byte_array, 2);
        no_byte_array = (unsigned char*)gds_read(fp, no_read, 2); 

		/* if we can't read 2 bytes, we must be at the end of the file. If we
		 * just get one byte and it's not  padding (a 0), something is wrong
		 * with the file structure... */
		if (no_read != 2)
		{
#ifdef DEBUG_GDSREADER
			printf ("# ***WHOOPS*** We seem to have reached the end of the file.\n");
#endif 
			if ((no_read == 1) && (no_byte_array[0] != 0))
			{
				printf ("# ***ERROR*** We read a single non-zero byte after the last record.\n");
				printf ("#             I'm suspecting that this isn't padding...\n");
			}
			break; /* this is how the loop exits on normal execution */
		}

		/* store the number of bytes as an integer */
		no_bytes = no_byte_array[0] * 256 + no_byte_array[1];

		/* we could be into the padding region at the end of the file... */
		if (no_bytes != 0)
		{
			/* make room for the rest of the record */
			//record = (unsigned char *) malloc (no_bytes - 2);

			/* read in the record */
			//no_read = ::read (fp, record, no_bytes - 2);
            record = (unsigned char*)gds_read(fp, no_read, no_bytes-2); 

			/* we should always get all of the bytes we asked for */
			if (no_read != no_bytes - 2)
			{
				/* Tell the user that there is a problem */
				printf ("# ***ERROR*** Couldn't read all of record.\n");
				printf ("#             It should have had %d bytes, could only read %d of them.\n",
						no_bytes, no_read + 2);
				printf ("#             This is a corrupt file...\n");

				/* dump what we got in raw form */
				for (corrupt_ktr = 0; corrupt_ktr < no_read; corrupt_ktr++)
				{
#ifdef DEBUG_GDSREADER
					printf ("0x%02x     # RAW\n", record[corrupt_ktr]);
#endif 
				}

				break;
			}

			/* now find the record type, numeric and ascii */
			record_type = record[0];
			find_record_type (record_type, enum_record_type, expected_data_type);

			/* find the data type, numeric and ascii */
			data_type = record[1];
			find_data_type (data_type, enum_data_type);

			/* if it's a ENDSTR or ENDEL, subtract from indent */
            if (enum_record_type == GdsRecords::ENDSTR
                    || enum_record_type == GdsRecords::ENDEL)
			{
                if (indent_amount >= 2)
                    indent_amount -= 2;
			}

#ifdef DEBUG_GDSREADER
			/* print it out */
			printf ("\n");
			printf ("%*s0x%04x     # RECORD_LENGTH              Bytes of data in this record\n",
					indent_amount, "", no_bytes);
			printf ("%*s0x%02x       # RECORD_TYPE:  %-12s %s\n",
					indent_amount, "", record_type, gds_record_ascii(enum_record_type), gds_record_description(enum_record_type));
			printf ("%*s0x%02x       # DATA_TYPE:    %-12s %s\n",
					indent_amount, "", data_type, gds_data_ascii(enum_data_type), gds_data_description(enum_data_type));
#endif 

			/* If the record and data types don't match, print an error, but
			 * keep right on processing. It seems redundant having both of
			 * these when it looks like the record type would be enough, but
			 * hey, I didn't create this format, I'm just parsing it and
			 * looking for errors... */
			if ((expected_data_type != 0xffff) && (expected_data_type != data_type))
			{
				find_data_type (expected_data_type, enum_expected_data_type);
				printf ("%*s# ***ERROR*** We were expecting data type 0x%02x (%s) to be specified, but\n",
						indent_amount, "", expected_data_type, gds_data_ascii(enum_expected_data_type));
				printf ("%*s#             data type 0x%02x (%s) was specified.\n",
						indent_amount, "", data_type, gds_data_ascii(enum_data_type));
				printf ("%*s#             I'll use the expected data type when I try to read this.\n",
						indent_amount, "");
			}

			/* now print the actual data (if present) */
			if (expected_data_type == GdsData::BIT_ARRAY)	/* BIT_ARRAY */
			{
				vector<int> vBitArray; vBitArray.reserve((no_read-2)>>1);
				for (data_ktr = 2; data_ktr < no_read; data_ktr += 2)
				{
                    /* use bit shifting instread of multiplication */
					bit_array = (record[data_ktr]<<8) + record[data_ktr + 1];
#ifdef DEBUG_GDSREADER
					printf ("%*s0x%04x     # DATA\n",
							indent_amount, "", bit_array);
					/* print a hopefully useful comment */
					print_bit_array_comments (enum_record_type, bit_array, indent_amount);
#endif 
					vBitArray.push_back(bit_array);
				}
				m_db.bit_array_cbk(enum_record_type, enum_data_type, vBitArray);
			}
			else if (expected_data_type == GdsData::INTEGER_2)	/* INTEGER_2 */
			{
				/* vInteger used to save data for callbacks, be careful, it should be int rather than unsigned int */
				vector<int> vInteger; vInteger.reserve((no_read-2)>>1);
				for (data_ktr = 2; data_ktr < no_read; data_ktr += 2)
				{
					display_integer = record[data_ktr];
					display_integer <<= 8;
					display_integer += record[data_ktr + 1];
#ifdef DEBUG_GDSREADER
					hex_display_integer = display_integer;
#endif 
					if (display_integer & 0x8000)	/* negative number, 2's
													 * comp */
					{
						display_integer &= 0x7fff;
						display_integer ^= 0x7fff;
						display_integer += 1;
						display_integer *= -1;
					}
#ifdef DEBUG_GDSREADER
					printf ("%*s0x%04x     # DATA: %d\n",
							indent_amount, "", hex_display_integer, display_integer);
#endif 
					vInteger.push_back(display_integer);
				}
				m_db.integer_2_cbk(enum_record_type, enum_data_type, vInteger);
			}
			else if (expected_data_type == GdsData::INTEGER_4)	/* INTEGER_4 */
			{
				/* vInteger used to save data for callbacks, be careful, it should be int rather than unsigned int */
				vector<int> vInteger; vInteger.reserve((no_read-2)>>2);
				for (data_ktr = 2; data_ktr < no_read; data_ktr += 4)
				{
					display_integer = 0;
					for (int_ktr = 0; int_ktr < 4; int_ktr++)
					{
						display_integer <<= 8;
						display_integer += record[data_ktr + int_ktr];
					}
#ifdef DEBUG_GDSREADER
					hex_display_integer = display_integer;
#endif 
					if (display_integer & 0x80000000)	/* negative number, 2's
														 * comp */
					{
						display_integer &= 0x7fffffff;
						display_integer ^= 0x7fffffff;
						display_integer += 1;
						display_integer *= -1;
					}
#ifdef DEBUG_GDSREADER
					printf ("%*s0x%08x # DATA: %d\n",
							indent_amount, "", hex_display_integer, display_integer);
#endif 
					vInteger.push_back(display_integer);
				}
				m_db.integer_4_cbk(enum_record_type, enum_data_type, vInteger);
			}
			else if (expected_data_type == GdsData::REAL_4)	/* REAL_4 */
			{
				vector<double> vFloat; vFloat.reserve((no_read-2)>>2);
				for (data_ktr = 2; data_ktr < no_read; data_ktr += 4)
				{
					real_sign = record[data_ktr] & 0x80;
					real_exponent = (record[data_ktr] & 0x7f) - 64;
					real_mantissa_int = 0;
					for (exponent_ktr = 1; exponent_ktr < 4; exponent_ktr++)
					{
						real_mantissa_int <<= 8;
						real_mantissa_int += record[data_ktr + exponent_ktr];
					}
					real_mantissa_float = (double) real_mantissa_int / pow (2, 24);
					display_float = real_mantissa_float * pow (16, (float) real_exponent);
					if (real_sign)
					{
						display_float *= -1;
					}
#ifdef DEBUG_GDSREADER
					printf ("%*s%-.9f # DATA\n", indent_amount, "", display_float);
#endif 
					vFloat.push_back(display_float);
				}
				m_db.real_4_cbk(enum_record_type, enum_data_type, vFloat);
			}
			else if (expected_data_type == GdsData::REAL_8)	/* REAL_8 */
			{
				vector<double> vFloat; vFloat.reserve((no_read-2)>>3);
				for (data_ktr = 2; data_ktr < no_read; data_ktr += 8)
				{
					real_sign = record[data_ktr] & 0x80;
					real_exponent = (record[data_ktr] & 0x7f) - 64;
					real_mantissa_int = 0;
					for (exponent_ktr = 1; exponent_ktr < 8; exponent_ktr++)
					{
						real_mantissa_int <<= 8;
						real_mantissa_int += record[data_ktr + exponent_ktr];
					}
					real_mantissa_float = (double) real_mantissa_int / pow (2, 56);
					display_float = real_mantissa_float * pow (16, (float) real_exponent);
					if (real_sign)
					{
						display_float *= -1;
					}
#ifdef DEBUG_GDSREADER
					printf ("%*s%-.18f # DATA\n", indent_amount, "", display_float);
#endif 
					vFloat.push_back(display_float);
				}
				m_db.real_8_cbk(enum_record_type, enum_data_type, vFloat);
			}
			else if (expected_data_type == GdsData::STRING)	/* STRING */
			{
				string str; str.reserve((no_read-2)>>1); 
				for (data_ktr = 2; data_ktr < no_read; data_ktr += 2)
				{
					display_char_1 = record[data_ktr];
					display_char_2 = record[data_ktr + 1];

					if (display_char_1 == '\0') break; /* quit early if encounter null character */
					else if (!isprint (display_char_1))
					{
						display_char_1 = '.';
					}
					str.push_back(display_char_1);

					if (display_char_2 == '\0') break; /* quit early if encounter null character */
					else if (!isprint (display_char_2))
					{
						display_char_2 = '.';
					}
					str.push_back(display_char_2);
#ifdef DEBUG_GDSREADER
					printf ("%*s0x%02x 0x%02x  # DATA: %c%c\n",
							indent_amount, "", record[data_ktr], record[data_ktr + 1],
							display_char_1, display_char_2);
#endif 
					//if (((!isprint (record[data_ktr])) && (record[data_ktr] != 0)) ||
					//		((!isprint (record[data_ktr + 1])) && (record[data_ktr + 1] != 0)))
					//{
					//	printf ("%*s# ***ERROR*** There was a non-printable character in the last 2 byte word.\n",
					//			indent_amount, "");
					//}
				}
				m_db.string_cbk(enum_record_type, enum_data_type, str);
			}
			else
			{
				if (expected_data_type != GdsData::NO_DATA)
				{
#ifdef DEBUG_GDSREADER
					for (data_ktr = 2; data_ktr < no_read; data_ktr++)
					{
						printf ("0x%02x # RAW(UNKNOWN)\n", record[data_ktr]);
					}
#endif 
				}
				/* enum_record_type == BGNSTR, BOUNDARY, PATH, BOX */
				else m_db.begin_end_cbk(enum_record_type); 
			}

			/* if it's a BGNSTR or the beginning of an element, add to indent */
			if (enum_record_type == GdsRecords::BGNSTR ||
					enum_record_type == GdsRecords::BOUNDARY ||
					enum_record_type == GdsRecords::PATH ||
					enum_record_type == GdsRecords::SREF ||
					enum_record_type == GdsRecords::AREF ||
					enum_record_type == GdsRecords::TEXT ||
					enum_record_type == GdsRecords::TEXTNODE ||
					enum_record_type == GdsRecords::NODE ||
					enum_record_type == GdsRecords::BOX)
			{
                indent_amount += NO_SPACES_TO_INDENT;
			}

			/* free the record memory */
			//free (record);
		}
		else
		{
#ifdef DEBUG_GDSREADER
			/* if it was a NULL record */
			printf ("%*s0x%04x # PADDING(NULL RECORD)\n",
					indent_amount, "", no_bytes);
#endif 
		}

	}

	return true;
}

void GdsReader::find_record_type (int numeric, GdsRecords::EnumType& record_name, int& expected_data_type)
{
    record_name = gds_record_type(numeric);
    expected_data_type = gds_record_expected_data(record_name);
}

void GdsReader::find_data_type (int numeric, GdsData::EnumType& data_name)
{
    data_name = gds_data_type(numeric);
}

void GdsReader::print_bit_array_comments (GdsRecords::EnumType enum_record_type, int bit_array, int indent_amount)
{
	int half_nyble;

	if (enum_record_type == GdsRecords::STRANS)
	{
		printf ("%*s           # bits 15 to 0, l to r\n",
				indent_amount, "");
		printf ("%*s           # 15=refl, 2=absmag, 1=absangle, others unused\n",
				indent_amount, "");
		if (bit_array & 0x8000)
		{
			printf ("%*s           # Reflected on X axis before rotation\n",
					indent_amount, "");
		}
		if (bit_array & 0x0002)
		{
			printf ("%*s           # Magnitude is absolute\n",
					indent_amount, "");
		}
		if (bit_array & 0x0001)
		{
			printf ("%*s           # Angle is absolute\n",
					indent_amount, "");
		}
		if (bit_array & 0x7ff8)
		{
			printf ("%*s           # ***ERROR*** Undefined bits set in STRANS\n",
					indent_amount, "");
		}
	}
	else if (enum_record_type == GdsRecords::PRESENTATION)
	{
		printf ("%*s           # bits 15 to 0, l to r\n",
				indent_amount, "");
		printf ("%*s           # bits 0 and 1: 00 left, 01 center, 10 right\n",
				indent_amount, "");
		printf ("%*s           # bits 2 and 3: 00 top 01, middle, 10 bottom\n",
				indent_amount, "");
		printf ("%*s           # bits 4 and 5: 00 font 0, 01 font 1, 10 font 2, 11 font 3,\n",
				indent_amount, "");
		half_nyble = bit_array;
		half_nyble &= 0x30;
		half_nyble >>= 4;
		printf ("%*s           # Font %d\n",
				indent_amount, "", half_nyble);
		half_nyble = bit_array;
		half_nyble &= 0x0c;
		half_nyble >>= 2;
		if (half_nyble == 0x00)
		{
			printf ("%*s           # Top justification\n",
					indent_amount, "");
		}
		else if (half_nyble == 0x01)
		{
			printf ("%*s           # Middle justification\n",
					indent_amount, "");
		}
		else if (half_nyble == 0x02)
		{
			printf ("%*s           # Bottom justification\n",
					indent_amount, "");
		}
		else
		{
			printf ("%*s           # ***ERROR*** Illegal justification\n",
					indent_amount, "");
		}
		half_nyble = bit_array;
		half_nyble &= 0x03;
		if (half_nyble == 0x00)
		{
			printf ("%*s           # Left justification\n",
					indent_amount, "");
		}
		else if (half_nyble == 0x01)
		{
			printf ("%*s           # Center justification\n",
					indent_amount, "");
		}
		else if (half_nyble == 0x02)
		{
			printf ("%*s           # Right justification\n",
					indent_amount, "");
		}
		else
		{
			printf ("%*s           # ***ERROR*** Illegal justification\n",
					indent_amount, "");
		}
		if (bit_array & 0xffc0)
		{
			printf ("%*s           # ***ERROR*** Undefined bits set in PRESENTATION\n",
					indent_amount, "");
		}
	}
	else if (enum_record_type == GdsRecords::ELFLAGS)
	{
		printf ("%*s           # bits 15 to 0, l to r\n",
				indent_amount, "");
		printf ("%*s           # 0=template, 1=external data, others unused\n",
				indent_amount, "");
		if (bit_array & 0x0001)
		{
			printf ("%*s           # This is template data\n",
					indent_amount, "");
		}
		if (bit_array & 0x0002)
		{
			printf ("%*s           # This is external data\n",
					indent_amount, "");
		}
		if (bit_array & 0xfffc)
		{
			printf ("%*s           # ***ERROR*** Undefined bits in ELFLAGS\n",
					indent_amount, "");
		}
	}
	else if (enum_record_type == GdsRecords::STRCLASS)
	{
		printf ("%*s           # Calma internal use only, should be all 0\n",
				indent_amount, "");
		printf ("%*s           # if this file was generated by non-Calma software\n",
				indent_amount, "");
		if (bit_array)
		{
			printf ("%*s           # ***WARNING*** The STRCLASS record type is used\n",
					indent_amount, "");
		}
	}
	else
	{
		printf ("\n");
	}

}

} // namespace GdsParser
