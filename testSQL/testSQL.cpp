#include "stdafx.h"
#include <iostream>

/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"
#include "mysql_driver.h" 
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/exception/all.hpp>

using namespace std;

int main(void)
{
	cout << endl << "Let's have MySQL count from 10 to 1..." << endl;

	try 
	{
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		sql::PreparedStatement *pstmt;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "123123");
		/* Connect to the MySQL test database */
		con->setSchema("test");

		stmt = con->createStatement();
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT)");
		delete stmt;

		/* '?' is the supported placeholder syntax */
		//pstmt = con->prepareStatement("INSERT INTO test(id) VALUES (?)");
		pstmt = con->prepareStatement("DELETE FROM test WHERE id=(?)");
		for (int i = 1; i <= 10; i++) 
		{
			pstmt->setInt(1,i);
			pstmt->executeUpdate();
		}
		delete pstmt;

		/* Select in ascending order */
		pstmt = con->prepareStatement("SELECT id FROM test ORDER BY id ASC");
		res = pstmt->executeQuery();

		/* Fetch in reverse = descending order! */
		res->afterLast();
		while (res->previous())
			cout << "\t... MySQL counts: " << res->getInt("id") << endl;
		delete res;

		delete pstmt;
		delete con;

	}
	catch (sql::SQLException &e) 
	{
		cout << "# ERR: SQLException in " << __FILE__;
		std::string a;
		a = boost::to_string(__FUNCTION__);
		cout << "\nBOOST:" << a << "\n";
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	cout << endl;
	system("pause");
	return EXIT_SUCCESS;
}