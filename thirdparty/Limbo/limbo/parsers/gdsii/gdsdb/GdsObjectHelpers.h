/**
 * @file   GdsObjectHelpers.h
 * @brief  Various GDSII objects and a top GDSII database 
 * @author Yibo Lin
 * @date   Oct 2016
 */

#ifndef LIMBO_PARSERS_GDSII_GDSDB_GDSOBJECTHELPERS_H
#define LIMBO_PARSERS_GDSII_GDSDB_GDSOBJECTHELPERS_H

#include <cmath>
#include <boost/geometry/strategies/transform.hpp>
#include <limbo/parsers/gdsii/stream/GdsReader.h>
#include <limbo/parsers/gdsii/stream/GdsWriter.h>
#include <limbo/preprocessor/Msg.h>

/// namespace for Limbo.GdsParser 
namespace GdsParser 
{
/// namespace for Limbo.GdsParser.GdsDB 
namespace GdsDB 
{

/// @brief a helper to facilitate actions on different GDSII objects.
/// 
/// Some helper functions to convert base class pointers to derived class pointers.  
/// The basic procedure is to cast the pointer and perform actions 
/// with template classes, we only need to implement copyable action classes. 
struct GdsObjectHelpers
{
    /// @brief API to run the actions over all GDSII objects 
    /// @tparam ActionType action type 
    /// @param type GDSII record 
    /// @param object GDSII object, which will be casted according to GDSII record 
    /// @param action user-defined action function object 
	template <typename ActionType>
	void operator()(::GdsParser::GdsRecords::EnumType type, GdsObject* object, ActionType action) const 
	{
		try
		{
			switch (type)
			{
				case ::GdsParser::GdsRecords::BOUNDARY:
					{
						GdsPolygon* ptr = dynamic_cast<GdsPolygon*>(object); 
						action(type, ptr); 
					}
					break;
				case ::GdsParser::GdsRecords::PATH:
					{
						GdsPath* ptr = dynamic_cast<GdsPath*>(object); 
						action(type, ptr); 
					}
					break;
				case ::GdsParser::GdsRecords::TEXT:
					{
						GdsText* ptr = dynamic_cast<GdsText*>(object); 
						action(type, ptr); 
					}
					break;
				case ::GdsParser::GdsRecords::SREF:
					{
						GdsCellReference* ptr = dynamic_cast<GdsCellReference*>(object); 
						action(type, ptr); 
					}
					break;
				case ::GdsParser::GdsRecords::AREF:
					{
						GdsCellArray* ptr = dynamic_cast<GdsCellArray*>(object); 
						action(type, ptr); 
					}
					break;
				default:
					limboAssertMsg(0, "unsupported type %d\n", type); 
			}
		}
		catch (std::exception& e)
		{
			limboPrint(limbo::kERROR, "exception in action %s: %s\n", action.message().c_str(), e.what()); 
			limboAssert(0);
		}
	}
};

/// an action function for copy a cell 
struct CopyCellObjectAction
{
	std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*>& target; ///< target cell 

	/// @brief constructor 
    /// @param t target cell 
	CopyCellObjectAction(std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*>& t) : target(t) {}
	/// @brief copy constructor 
    /// @param rhs a CopyCellObjectAction object 
	CopyCellObjectAction(CopyCellObjectAction const& rhs) : target(rhs.target) {}

    /// @brief API to run the copy action 
    /// @tparam ObjectType GDSII object type 
    /// @param type GDSII record 
    /// @param object GDSII object 
	template <typename ObjectType>
	void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
	{
		target.first = type;
		target.second = new ObjectType(*object); 
	}

	/// @return a message of action for debug 
	std::string message() const 
	{
		return "CopyCellObjectAction"; 
	}
}; 

/// an action function for delete a cell 
struct DeleteCellObjectAction
{
	std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*>& target; ///< target cell 

	/// @brief constructor 
    /// @param t target cell 
	DeleteCellObjectAction(std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*>& t) : target(t) {}
	/// @brief copy constructor 
    /// @param rhs a CopyCellObjectAction object 
	DeleteCellObjectAction(DeleteCellObjectAction const& rhs) : target(rhs.target) {}

    /// @brief API to run the copy action 
    /// @tparam ObjectType GDSII object type 
    /// @param object GDSII object 
	template <typename ObjectType>
	void operator()(::GdsParser::GdsRecords::EnumType /*type*/, ObjectType* object)
	{
		delete object; 
	}

	/// @return a message of action for debug 
	std::string message() const 
	{
		return "DeleteCellObjectAction"; 
	}
}; 

/// an action function for write a cell 
/// @tparam GdsWriterType object writer type 
template <typename GdsWriterType>
struct WriteCellObjectAction
{
	GdsWriterType const& writer; ///< wrapper of GDSII writer which will invoke ::GdsParser::GdsWriter to write 
	::GdsParser::GdsWriter& gdsWriter; ///< GDSII writer 
	std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*> const& target; ///< target object 

	/// @brief constructor 
    /// @param w GDSII object writer 
    /// @param gw GDSII writer 
    /// @param t target object 
	WriteCellObjectAction(GdsWriterType const& w, ::GdsParser::GdsWriter& gw, std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*> const& t) : writer(w), gdsWriter(gw), target(t) {}
	/// @brief copy constructor 
    /// @param rhs a WriteCellObjectAction object 
	WriteCellObjectAction(WriteCellObjectAction const& rhs) : writer(rhs.writer), gdsWriter(rhs.gdsWriter), target(rhs.target) {}

    /// @brief API to run the writing action 
    /// @tparam ObjectType object type 
    /// @param object GDSII object 
	template <typename ObjectType>
	void operator()(::GdsParser::GdsRecords::EnumType /*type*/, ObjectType* object)
	{
		writer.write(gdsWriter, *object); 
	}

	/// @return a message of action for debug 
	std::string message() const 
	{
		return "WriteCellObjectAction"; 
	}
}; 

/// @namespace GdsParser::GdsDB::ExtractCellObjectActionDetails 
/// @brief Detailed action functions for extract objects of a cell 
namespace ExtractCellObjectActionDetails 
{

/// default action is to copy objects 
/// @tparam ObjectType GDSII object type 
/// @param targetCell target cell 
/// @param type GDSII record 
/// @param object GDSII object in the cell 
template <typename ObjectType>
inline void extract(GdsDB const& /*gdsDB*/, GdsCell const& /*srcCell*/, GdsCell& targetCell, ::GdsParser::GdsRecords::EnumType type, ObjectType* object)
{
	ObjectType* ptr = new ObjectType (*object); 
	targetCell.objects().push_back(std::make_pair(type, ptr)); 
}

/// specialization for SREF 
/// @param gdsDB GDSII database 
/// @param srcCell source cell with the SREF object 
/// @param targetCell target cell 
/// @param type GDSII record 
/// @param object the GDSII SREF object in the cell 
template <>
inline void extract<GdsCellReference>(GdsDB const& gdsDB, GdsCell const& srcCell, GdsCell& targetCell, ::GdsParser::GdsRecords::EnumType type, GdsCellReference* object)
{
	limboAssert(type == ::GdsParser::GdsRecords::SREF);
	limboAssertMsg(srcCell.name() != object->refCell(), "self reference of cell %s", srcCell.name().c_str());
	GdsCell const* refCell = gdsDB.getCell(object->refCell()); 
	limboAssertMsg(refCell, "failed to find reference cell %s", object->refCell().c_str());
	// generate a new cell from reference 
	GdsCell cell = object->extractCellRef(gdsDB, *refCell); 
	// directly append object pointers to target cell 
	targetCell.objects().insert(targetCell.objects().end(), cell.objects().begin(), cell.objects().end()); 
	// must clear the list, otherwise, the pointers are destroyed 
	cell.objects().clear(); 
}

/// specialization for AREF 
/// @param type GDSII record 
/// @todo finish extraction of GDSII AREF 
template <>
inline void extract<GdsCellArray>(GdsDB const& /*gdsDB*/, GdsCell const& /*srcCell*/, GdsCell& /*targetCell*/, ::GdsParser::GdsRecords::EnumType type, GdsCellArray* /*object*/)
{
	limboAssert(type == ::GdsParser::GdsRecords::AREF);
	limboAssertMsg(0, "not implemented yet"); 
}

} // namespace ExtractCellObjectActionDetails

/// @brief Action function object to extract cell. 
/// 
/// Given GDSII database and source cell, construct a target cell with all the SREF and AREF replaced as copies   
struct ExtractCellObjectAction
{
	GdsDB const& gdsDB; ///< GDSII database 
	GdsCell const& srcCell; ///< source cell 
	GdsCell& targetCell; ///< target cell 

	/// @brief constructor 
    /// @param db GDSII database 
    /// @param sc source cell 
    /// @param tc target cell 
	ExtractCellObjectAction(GdsDB const& db, GdsCell const& sc, GdsCell& tc) : gdsDB(db), srcCell(sc), targetCell(tc) {}
	/// @brief copy constructor 
    /// @param rhs an object 
	ExtractCellObjectAction(ExtractCellObjectAction const& rhs) : gdsDB(rhs.gdsDB), srcCell(rhs.srcCell), targetCell(rhs.targetCell) {}

    /// @brief API to run the extraction 
    /// 
	/// Since template specialization for member function is not supported 
	/// I have to use external function within a namespace @ref GdsParser::GdsDB::ExtractCellObjectActionDetails
    /// 
    /// @param type GDSII record 
    /// @param object GDSII object 
	template <typename ObjectType>
	void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
	{
		ExtractCellObjectActionDetails::extract(gdsDB, srcCell, targetCell, type, object);
	}

	/// @return a message of action for debug 
	std::string message() const 
	{
		return "ExtractCellObjectAction"; 
	}
};

/// @namespace GdsParser::GdsDB::ApplyCellReferenceActionDetails 
/// @brief An action function to apply changes to a copied cell reference. 
/// It contains various transformers. 
namespace ApplyCellReferenceActionDetails 
{

/// @brief Translate operation 
struct Translate 
{
	GdsCellReference::point_type offset; ///< offset 

    /// @brief constructor 
    /// @param o offset 
	Translate(GdsCellReference::point_type const& o) : offset(o) {}

    /// @brief API to run operation.
    /// @param p given position and perform operation on the position 
	void operator()(GdsCellReference::point_type& p) const 
	{
		p = gtl::construct<GdsCellReference::point_type>(p.x()+offset.x(), p.y()+offset.y());
	}
};
/// @brief Rotate operation 
struct Rotate 
{
	double angle; ///< angle 
	double cosAngle; ///< cosine value of angle 
	double sinAngle; ///< sine value of angle 

    /// @brief constructor 
    /// @param a angle 
	Rotate(double a) : angle(a) 
	{
		cosAngle = cos(angle/180.0*M_PI);
		sinAngle = sin(angle/180.0*M_PI);
	}

    /// @brief API to run operation.  
	/// Transformation matrix \n
	/// cos(theta) -sin(theta) \n
	/// sin(theta) cos(theta)
    /// @param p given position and perform operation on the position 
	void operator()(GdsCellReference::point_type& p) const 
	{
		p = gtl::construct<GdsCellReference::point_type>(
				p.x()*cosAngle - p.y()*sinAngle, 
				p.x()*sinAngle + p.y()*cosAngle
				);
	}
};
/// @brief Scale magnification 
struct MagScale 
{
	double scaleX; ///< scale x 
	double scaleY; ///< scale y 

    /// @brief constructor
    /// @param sx scale x
    /// @param sy scale y 
	MagScale(double sx, double sy) : scaleX(sx), scaleY(sy) {}

    /// @brief API to run operation 
    /// @param p given position and perform operation on the position 
	void operator()(GdsCellReference::point_type& p) const 
	{
		p = gtl::construct<GdsCellReference::point_type>(p.x()*scaleX, p.y()*scaleY);
	}
};
/// @brief X reflection 
struct XReflection
{
    /// @brief API to run operation. 
    /// @param p given position and perform operation on the position 
	void operator()(GdsCellReference::point_type& p) const 
	{
		p = gtl::construct<GdsCellReference::point_type>(p.x(), -p.y());
	}
};

/// @brief Transform operation over an array 
/// @tparam Iterator iterator to object 
/// @tparam TransformerType transformer type 
/// @param first, last begin and end iterator to object 
/// @param transform transform function object 
template <typename Iterator, typename TransformerType>
inline void transform(Iterator first, Iterator last, TransformerType transform)
{
	for (; first != last; ++first)
		transform(*first);
}

/// @brief copy points of objects to array 
/// @tparam ObjectType GDSII object type 
/// @param vPoint reference to point array 
/// @param object GDSII object 
template <typename ObjectType>
inline void copyToArray(std::vector<GdsCellReference::point_type>& vPoint, ObjectType* object)
{
	vPoint.assign(object->begin(), object->end());
}
/// @brief copy points of objects to array 
/// @param vPoint reference to point array 
/// @param object GDSII object 
template <>
inline void copyToArray<GdsText>(std::vector<GdsCellReference::point_type>& vPoint, GdsText* object)
{
	vPoint.assign(1, object->position());
}
/// @brief copy points of objects from array 
/// @tparam ObjectType GDSII object type 
/// @param vPoint point array 
/// @param object GDSII object 
template <typename ObjectType>
inline void copyFromArray(std::vector<GdsCellReference::point_type> const& vPoint, ObjectType* object)
{
	object->set(vPoint.begin(), vPoint.end());
}
/// @brief copy points of objects from array 
/// @param vPoint point array 
/// @param object GDSII object 
template <>
inline void copyFromArray<GdsText>(std::vector<GdsCellReference::point_type> const& vPoint, GdsText* object)
{
	object->setPosition(vPoint.front());
}

/// @brief apply cell reference 
/// @tparam ObjectType GDSII object type 
/// @param cellRef CREF object 
/// @param object GDSII object 
template <typename ObjectType>
inline void apply(GdsCellReference const& cellRef, ObjectType* object)
{
	std::vector<GdsCellReference::point_type> vPoint; 
	copyToArray(vPoint, object);
	// the order must be kept according to the manual 
	// strans
	if (cellRef.strans() != std::numeric_limits<int>::max() && cellRef.strans()/32768 > 0) // apply x reflection 
	{
		transform(vPoint.begin(), vPoint.end(), XReflection()); 
	}
	// magnification 
	if (cellRef.magnification() != std::numeric_limits<double>::max())
	{
		transform(vPoint.begin(), vPoint.end(), MagScale(cellRef.magnification(), cellRef.magnification())); 
	}
	// angle 
	if (cellRef.angle() != std::numeric_limits<double>::max())
	{
		transform(vPoint.begin(), vPoint.end(), Rotate(cellRef.angle())); 
	}
	// position offset 
	if (cellRef.position().x() != std::numeric_limits<int>::max() && cellRef.position().y() != std::numeric_limits<int>::max())
	{
		transform(vPoint.begin(), vPoint.end(), Translate(cellRef.position())); 
	}
	copyFromArray(vPoint, object);
}

/// @brief no reference to cell reference; it should not reach here 
template <>
inline void apply<GdsCellReference>(GdsCellReference const& /*cellRef*/, GdsCellReference* /*object*/)
{
	limboAssertMsg(0, "should not arrive here"); 
}

/// @brief no reference to cell array; it should not reach here 
template <>
inline void apply<GdsCellArray>(GdsCellReference const& /*cellRef*/, GdsCellArray* /*object*/)
{
	limboAssertMsg(0, "should not arrive here"); 
}

} // namespace ApplyCellReferenceActionDetails

/// @brief Apply cell reference action. 
/// It needs to call functions in @ref GdsParser::GdsDB::ApplyCellReferenceActionDetails
struct ApplyCellReferenceAction
{
	GdsCellReference const& cellRef; ///< CREF object 

	/// @brief constructor 
    /// @param cr CREF object 
	ApplyCellReferenceAction(GdsCellReference const& cr) : cellRef(cr) {}
	/// copy constructor 
    /// @param rhs an ApplyCellReferenceAction object 
	ApplyCellReferenceAction(ApplyCellReferenceAction const& rhs) : cellRef(rhs.cellRef) {}

    /// @brief API to run the action 
    /// @tparam ObjectType GDSII object type 
    /// @param object GDSII object 
	template <typename ObjectType>
	void operator()(::GdsParser::GdsRecords::EnumType /*type*/, ObjectType* object)
	{
		ApplyCellReferenceActionDetails::apply(cellRef, object);
	}

	/// @return a message of action for debug 
	std::string message() const 
	{
		return "ApplyCellReferenceAction"; 
	}
};

} // namespace GdsDB
} // namespace GdsParser

#endif
