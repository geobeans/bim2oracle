/* Copyright (c) 2001, 2010, Oracle and/or its affiliates.
All rights reserved. */
/*
NAME
occidml.cpp - Basic DML Operations demo

DESCRIPTION
To exhibit the insertion, selection, updating and deletion of
a row using OCCI interface

MODIFIED   (MM/DD/YY)
mvasudev   09/27/10 - remove HR schema and user SCOTT
mvasudev   05/22/08 - Add try/catch blocks
sudsrini   10/22/06 - Username/Password lower case
lburgess   04/14/06 - lowercase passwords
sudsrini   07/23/04 - Copyright Info
idcqe      03/05/01 - Creation

*/

#define BOOST_DATE_TIME_NO_LIB
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>


#include <ifcpp/geometry/Carve/GeometryConverter.h>
//#include <ifcpp/geometry/Carve/ConverterOSG.h>
#include <ifcpp/model/BuildingModel.h>
#include <ifcpp/model/BuildingException.h>
#include <ifcpp/model/BuildingGuid.h>
#include <ifcpp/model/AttributeObject.h>
#include <ifcpp/reader/ReaderSTEP.h>
#include <ifcpp/reader/ReaderUtil.h>
#include <ifcpp/writer/WriterSTEP.h>
#include <ifcpp/IFC4/include/IfcProduct.h>
#include <ifcpp/IFC4/include/IfcSite.h>
#include <ifcpp/IFC4/include/IfcLengthMeasure.h>
#include <ifcpp/IFC4/include/IfcOpeningElement.h>
#include <ifcpp/IFC4/include/IfcOwnerHistory.h>
#include <ifcpp/IFC4/include/IfcGloballyUniqueId.h>

//#ifdef WIN32 
//#include <WTypes.h> // Include this first on Win (bug #35683) 
//#endif
//
//#include <windows.h>
#include <atlstr.h>

#include <iostream>
#include <occi.h>

//#include <windows.h>
using namespace oracle::occi;
using namespace std;

int g_id = 1;
//BOOL StringToWString(const std::string &str, std::wstring &wstr)
//{
//	int nLen = (int)str.length();
//	wstr.resize(nLen, L' ');
//
//	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);
//
//	if (nResult == 0)
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>




class  occidml
{
private:

	Environment *env;
	Connection *conn;
	Statement *stmt;
public:

	occidml(string user, string passwd, string db)
	{
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, passwd, db);
	}

	~occidml()
	{
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}

	/**
	* Insertion of a row with dynamic binding, PreparedStatement functionality.
	*/
	void insertBind(int c1, string c2, string c3, string c4, string c5, string c6)
	{
		string sqlStmt = "INSERT INTO author_tab VALUES (:v1, :v2, :v3, :v4, :v5, :v6)";
		stmt = conn->createStatement(sqlStmt);
		try{
			stmt->setInt(1, c1);
			stmt->setString(2, c2);
			stmt->setString(3, c3);
			stmt->setString(4, c4);
			stmt->setString(5, c5);
			stmt->setString(6, c6);
			stmt->executeUpdate();
			cout << "insert - Success" << endl;
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for insertBind" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}

		conn->terminateStatement(stmt);
	}

	/**
	* Inserting a row into the table.
	*/
	void insertRow()
	{
		string sqlStmt = "INSERT INTO author_tab VALUES (111, 'ASHOK')";
		stmt = conn->createStatement(sqlStmt);
		try{
			stmt->executeUpdate();
			cout << "insert - Success" << endl;
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for insertRow" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}

		conn->terminateStatement(stmt);
	}

	/**
	* updating a row
	*/
	void updateRow(int c1, string c2)
	{
		string sqlStmt =
			"UPDATE author_tab SET author_name = :x WHERE author_id = :y";
		stmt = conn->createStatement(sqlStmt);
		try{
			stmt->setString(1, c2);
			stmt->setInt(2, c1);
			stmt->executeUpdate();
			cout << "update - Success" << endl;
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for updateRow" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}

		conn->terminateStatement(stmt);
	}


	/**
	* deletion of a row
	*/
	void deleteRow(int c1, string c2)
	{
		string sqlStmt =
			"DELETE FROM author_tab WHERE author_id= :x AND author_name = :y";
		stmt = conn->createStatement(sqlStmt);
		try{
			stmt->setInt(1, c1);
			stmt->setString(2, c2);
			stmt->executeUpdate();
			cout << "delete - Success" << endl;
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for deleteRow" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}

		conn->terminateStatement(stmt);
	}

	/**
	* displaying all the rows in the table
	*/
	void displayAllRows()
	{
		string sqlStmt = "SELECT * FROM GB_LAYERS";
		stmt = conn->createStatement(sqlStmt);
		ResultSet *rset = stmt->executeQuery();
		try{
			while (rset->next())
			{
				cout << "author_id: " << rset->getInt(1) << "  author_name: "
					<< rset->getString(2) << endl;
			}
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for displayAllRows" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}

		stmt->closeResultSet(rset);
		conn->terminateStatement(stmt);
	}

	/**
	* Inserting a row into elements table.
	* Demonstrating the usage of BFloat and BDouble datatypes
	*/
	void insertElement(string elm_name, float mvol = 0.0, double awt = 0.0)
	{
		BFloat mol_vol;
		BDouble at_wt;

		if (!(mvol))
			mol_vol.isNull = TRUE;
		else
			mol_vol.value = mvol;

		if (!(awt))
			at_wt.isNull = TRUE;
		else
			at_wt.value = awt;

		string sqlStmt = "INSERT INTO elements VALUES (:v1, :v2, :v3)";
		stmt = conn->createStatement(sqlStmt);

		try{
			stmt->setString(1, elm_name);
			stmt->setBFloat(2, mol_vol);
			stmt->setBDouble(3, at_wt);
			stmt->executeUpdate();
			cout << "insertElement - Success" << endl;
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for insertElement" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}
		conn->terminateStatement(stmt);
	}

	/**
	* displaying rows from element table
	*/
	void displayElements()
	{
		string sqlStmt =
			"SELECT element_name, molar_volume, atomic_weight FROM elements \
			    order by element_name";
		stmt = conn->createStatement(sqlStmt);
		ResultSet *rset = stmt->executeQuery();
		try{
			cout.precision(7);
			while (rset->next())
			{
				string elem_name = rset->getString(1);
				BFloat mol_vol = rset->getBFloat(2);
				BDouble at_wt = rset->getBDouble(3);

				cout << "Element Name: " << elem_name << endl;

				if (mol_vol.isNull)
					cout << "Molar Volume is NULL" << endl;
				else
					cout << "Molar Volume: " << mol_vol.value << " cm3 mol-1" << endl;

				if (at_wt.isNull)
					cout << "Atomic Weight is NULL" << endl;
				else
					cout << "Atomic Weight: " << at_wt.value << " g/mole" << endl;
			}
		}
		catch (SQLException ex)
		{
			cout << "Exception thrown for displayElements" << endl;
			cout << "Error number: " << ex.getErrorCode() << endl;
			cout << ex.getMessage() << endl;
		}

		stmt->closeResultSet(rset);
		conn->terminateStatement(stmt);
	}

}; // end of class  occidml

void PorcessEntity(shared_ptr<BuildingEntity> entity)
{
	if (!entity)
	{
		return;
	}
	std::vector<std::pair<std::string, shared_ptr<BuildingObject> > > vec_attributes;
	entity->getAttributes(vec_attributes);
	if (vec_attributes.size() > 4)
	{
		// check if the object has relevant pointers
		size_t num_relevant_attributes = 0;
		// first 4 attributes are GlobalId, OwnerHistory, Name, Description
		for (size_t ii = 0; ii < vec_attributes.size(); ++ii)
		{
			shared_ptr<BuildingObject> attribute = vec_attributes[ii].second;
			if (attribute)
			{
				shared_ptr<AttributeObjectVector> attribute_vec = dynamic_pointer_cast<AttributeObjectVector>(attribute);
				if (attribute_vec)
				{
					if (attribute_vec->m_vec.size() > 0)
					{
						++num_relevant_attributes;
						std::cout << CStringA(attribute_vec->toString().data()).GetString() << std::endl;
					}
				}
				else
				{
					shared_ptr<BuildingEntity> attribute_entity = dynamic_pointer_cast<BuildingEntity>(attribute);
					if (attribute_entity)
					{
						++num_relevant_attributes;
						std::string str = "test";
						std::cout << CStringA(attribute_entity->toString().data()).GetString() << std::endl;

						PorcessEntity(attribute_entity);
					}
				}
			}
		}
	}
}

void InsertEntityProperty(shared_ptr<BuildingObject> obj, std::unordered_set<int>& set_visited, occidml *dml)
{
	shared_ptr<IfcObjectDefinition> obj_def = dynamic_pointer_cast<IfcObjectDefinition>(obj);
	if (obj_def)
	{
		if (set_visited.find(obj_def->m_entity_id) != set_visited.end())
		{
			return;
		}
		set_visited.insert(obj_def->m_entity_id);

		std::cout << CStringA(obj_def->toString().data()).GetString() << std::endl;
	
		std::vector<std::pair<std::string, shared_ptr<BuildingObject> > > vec_attributes;
		obj_def->getAttributes(vec_attributes);

		// first 4 attributes are GlobalId, OwnerHistory, Name, Description
		for (size_t ii = 0; ii < vec_attributes.size(); ++ii)
		{
			shared_ptr<BuildingObject> attribute = vec_attributes[ii].second;
			std::cout << vec_attributes[ii].first << " ";
			string strCode = "-";
			string strField = vec_attributes[ii].first;
			string strClass = CStringA(obj_def->toString().data()).GetString();
			string strValue = "-";
			string strUnit = "-";
			if (attribute)
			{
				shared_ptr<BuildingObject> attribute_entity = dynamic_pointer_cast<BuildingObject>(attribute);
				if (attribute_entity)
				{
					std::cout << CStringA(attribute_entity->toString().data()).GetString() << std::endl;
					InsertEntityProperty(attribute_entity, set_visited, dml);
				}
				else
				{
					std::cout << CStringA(attribute->toString().data()).GetString() << std::endl;
					strValue = CStringA(attribute->toString().data()).GetString();					
				}

			}
			else
			{
				std::cout << std::endl;
			}
			try{
				dml->insertBind(g_id, strCode, strClass, strField, strUnit, strValue);
			}
			catch (SQLException ex){
				cout << ex.getMessage() << endl;
			}

			g_id++;
		}
	}
}

void PorcessEntityProperty(shared_ptr<BuildingObject> obj, std::unordered_set<int>& set_visited, occidml *dml)
{
	std::vector<shared_ptr<IfcObjectDefinition> >::iterator it_object_def;

	shared_ptr<IfcObjectDefinition> obj_def = dynamic_pointer_cast<IfcObjectDefinition>(obj);
	if (obj_def)
	{
		if (set_visited.find(obj_def->m_entity_id) != set_visited.end())
		{
			return;
		}
		set_visited.insert(obj_def->m_entity_id);
		
		std::cout << CStringA(obj_def->toString().data()).GetString() << std::endl;
		//item = new QTreeWidgetItem();

		//if (obj_def->m_Name)
		//{
		//	if (obj_def->m_Name->m_value.size() > 0)
		//	{
		//		item->setText(0, QString::fromStdWString(obj_def->m_Name->m_value));
		//	}
		//}

		//item->setText(1, QString::number(obj_def->m_entity_id));
		//item->setText(2, obj_def->className());

		std::vector<std::pair<std::string, shared_ptr<BuildingObject> > > vec_attributes;
		obj_def->getAttributes(vec_attributes);

		// first 4 attributes are GlobalId, OwnerHistory, Name, Description
		for (size_t ii = 0; ii < vec_attributes.size(); ++ii)
		{
			shared_ptr<BuildingObject> attribute = vec_attributes[ii].second;
			std::cout << vec_attributes[ii].first << " ";
			string strCode = "-";
			string strField = vec_attributes[ii].first;
			string strClass = CStringA(obj_def->toString().data()).GetString();
			string strValue = "-";
			string strUnit = "-";
			if (attribute)
			{
				std::cout << CStringA(attribute->toString().data()).GetString() << std::endl;
				strValue = CStringA(attribute->toString().data()).GetString();
				/*shared_ptr<AttributeObjectVector> attribute_vec = dynamic_pointer_cast<AttributeObjectVector>(attribute);
				if (attribute_vec)
				{
					if (attribute_vec->m_vec.size() > 0)
					{
						std::cout << CStringA(attribute_vec->toString().data()).GetString() << std::endl;
					}
				}
				else
				{
					shared_ptr<BuildingEntity> attribute_entity = dynamic_pointer_cast<BuildingEntity>(attribute);
					if (attribute_entity)
					{
						std::string str = "test";
						std::cout << CStringA(attribute_entity->toString().data()).GetString() << std::endl;

						PorcessEntity(attribute_entity);
					}
				}*/
			}
			else
			{
				std::cout << std::endl;
			}
			try{
				dml->insertBind(g_id, strCode,strClass,strField,strUnit,strValue);
			}
			catch (SQLException ex){
				cout << ex.getMessage() << endl;
			}

			g_id++;
		}



		if (obj_def->m_IsDecomposedBy_inverse.size() > 0)
		{
			std::vector<weak_ptr<IfcRelAggregates> >& vec_IsDecomposedBy = obj_def->m_IsDecomposedBy_inverse;
			std::vector<weak_ptr<IfcRelAggregates> >::iterator it;
			for (it = vec_IsDecomposedBy.begin(); it != vec_IsDecomposedBy.end(); ++it)
			{
				shared_ptr<IfcRelAggregates> rel_agg(*it);
				std::vector<shared_ptr<IfcObjectDefinition> >& vec = rel_agg->m_RelatedObjects;

				for (it_object_def = vec.begin(); it_object_def != vec.end(); ++it_object_def)
				{
					shared_ptr<IfcObjectDefinition> child_obj_def = (*it_object_def);
					PorcessEntityProperty(child_obj_def, set_visited,dml);
				}
			}
		}

		shared_ptr<IfcSpatialStructureElement> spatial_ele = dynamic_pointer_cast<IfcSpatialStructureElement>(obj_def);
		if (spatial_ele)
		{
			std::vector<weak_ptr<IfcRelContainedInSpatialStructure> >& vec_contained = spatial_ele->m_ContainsElements_inverse;
			if (vec_contained.size() > 0)
			{
				std::vector<weak_ptr<IfcRelContainedInSpatialStructure> >::iterator it_rel_contained;
				for (it_rel_contained = vec_contained.begin(); it_rel_contained != vec_contained.end(); ++it_rel_contained)
				{
					shared_ptr<IfcRelContainedInSpatialStructure> rel_contained(*it_rel_contained);
					std::vector<shared_ptr<IfcProduct> >& vec_related_elements = rel_contained->m_RelatedElements;
					std::vector<shared_ptr<IfcProduct> >::iterator it;

					for (it = vec_related_elements.begin(); it != vec_related_elements.end(); ++it)
					{
						shared_ptr<IfcProduct> related_product = (*it);

						PorcessEntityProperty(related_product, set_visited,dml);
					}
				}
			}
		}
	}
}

int main(void)
{
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); // 这里是两个() ，因为这里是调用的 () 的运算符重载
	const string tmp_uuid = boost::uuids::to_string(a_uuid);

	string user = "TEST_TEMP";
	string passwd = "TEST_TEMP";
	string db = "192.168.111.200:1521/GISDB";
	occidml *demo = NULL;
	try{
		cout << "occidml - Exhibiting simple insert, delete & update operations"
			<< endl;
		demo = new occidml(user, passwd, db);
	}
	catch (SQLException ex){
		cout << ex.getMessage() << endl;
	}
	cout << "occidml - done" << endl;

	shared_ptr<BuildingModel> m_ifc_model = shared_ptr<BuildingModel>(new BuildingModel());
	shared_ptr<GeometrySettings> geom_settings(new GeometrySettings());
	shared_ptr<GeometryConverter> m_geometry_converter = shared_ptr<GeometryConverter>(new GeometryConverter(m_ifc_model));
	shared_ptr<ReaderSTEP> m_step_reader = shared_ptr<ReaderSTEP>(new ReaderSTEP());
	shared_ptr<WriterSTEP>	 m_step_writer = shared_ptr<WriterSTEP>(new WriterSTEP());

	m_geometry_converter->clearMessagesCallback();
	std::string strIFC = "xxx.ifc";
	std::wstring wstrIFC;
	//StringToWString(strIFC, wstrIFC);
	m_step_reader->loadModelFromFile(/*wstrIFC*/L"E:\\prjs\\osg34_vs2013\\bin\\wlm.ifc", m_geometry_converter->getBuildingModel());
	
	std::unordered_set<int> set_visited;
	shared_ptr<IfcProject> project = m_ifc_model->getIfcProject();
	if (project)
	{
		PorcessEntityProperty(project, set_visited, demo);
	}
	
	try{
		delete (demo);
	}
	catch (SQLException ex){
		cout << ex.getMessage() << endl;
	}

	//auto map_ifc_objects = m_ifc_model->getMapIfcEntities();
	//for (auto it = map_ifc_objects.begin(); it != map_ifc_objects.end(); ++it)
	//{
	//	shared_ptr<BuildingEntity> entity = it->second;
	//	if (!entity)
	//	{
	//		continue;
	//	}

	//	shared_ptr<IfcSite> site = dynamic_pointer_cast<IfcSite>(entity);

	//	if (!site) continue;
	//	std::cout << CStringA(entity->toString().data()).GetString() << std::endl;

	//	std::vector<std::pair<std::string, shared_ptr<BuildingObject> > > vec_attributes;
	//	entity->getAttributes(vec_attributes);
	//	if (vec_attributes.size() > 4)
	//	{
	//		// check if the object has relevant pointers
	//		size_t num_relevant_attributes = 0;
	//		// first 4 attributes are GlobalId, OwnerHistory, Name, Description
	//		for (size_t ii = 0; ii < vec_attributes.size(); ++ii)
	//		{
	//			shared_ptr<BuildingObject> attribute = vec_attributes[ii].second;
	//			if (attribute)
	//			{
	//				shared_ptr<AttributeObjectVector> attribute_vec = dynamic_pointer_cast<AttributeObjectVector>(attribute);
	//				if (attribute_vec)
	//				{
	//					if (attribute_vec->m_vec.size() > 0)
	//					{
	//						++num_relevant_attributes;
	//						std::cout << CStringA(attribute_vec->toString().data()).GetString() << std::endl;
	//					}
	//				}
	//				else
	//				{
	//					shared_ptr<BuildingEntity> attribute_entity = dynamic_pointer_cast<BuildingEntity>(attribute);
	//					if (attribute_entity)
	//					{
	//						++num_relevant_attributes;
	//						std::string str = "test";
	//						std::cout << CStringA(attribute_entity->toString().data()).GetString() << std::endl;
	//						PorcessEntity(attribute_entity);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}
