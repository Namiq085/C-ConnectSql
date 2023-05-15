#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
using namespace std;

int main() {

#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000

    //define handles and variables
    SQLHANDLE sqlConnHandle;
    SQLHANDLE sqlStmtHandle;
    SQLHANDLE sqlEnvHandle;
    SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];

    //initializations
    sqlConnHandle = NULL;
    sqlStmtHandle = NULL;

    //allocations
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
        goto COMPLETED;

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
        goto COMPLETED;

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
        goto COMPLETED;

    //output
    cout << "Attempting connection to SQL Server...";
    cout << "\n";

    //connect to SQL Server  
    //I am using a trusted connection and port 14808
    //it does not matter if you are using default or named instance
    //just make sure you define the server name and the port
    //You have the option to use a username/password instead of a trusted connection
    //but is more secure to use a trusted connection
    switch (SQLDriverConnect(sqlConnHandle,
        NULL,
        //(SQLWCHAR*)L"DRIVER={SQL Server};SERVER=localhost, 1433;DATABASE=master;UID=username;PWD=password;",
        (SQLWCHAR*)L"DRIVER={SQL Server};SERVER=NAMIQ\\SQLEXPRESS01;DATABASE=Tenslog;Trusted=true;",
        SQL_NTS,
        retconstring,
        1024,
        NULL,
        SQL_DRIVER_NOPROMPT)) {

    case SQL_SUCCESS:
        cout << "Successfully connected to SQL Server";
        cout << "\n";
        break;

    case SQL_SUCCESS_WITH_INFO:
        cout << "Successfully connected to SQL Server";
        cout << "\n";
        break;

    case SQL_INVALID_HANDLE:
        cout << "Could not connect to SQL Server";
        cout << "\n";
        goto COMPLETED;

    case SQL_ERROR:
        cout << "Could not connect to SQL Server";
        cout << "\n";
        goto COMPLETED;

    default:
        break;
    }

    //if there is a problem connecting then exit application
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
        goto COMPLETED;

    //output
    cout << "\n";
    cout << "Executing T-SQL query...";
    cout << "\n";

    //if there is a problem executing the query then exit application
    //else display query result
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"SELECT UserId, Description FROM Supports", SQL_NTS)) {
        cout << "Error querying SQL Server";
        cout << "\n";
        goto COMPLETED;
    }
    else {

        //declare output variable and pointer
        SQLCHAR sqlVersion[SQL_RESULT_LEN];
        SQLINTEGER ptrSqlVersion;

        SQLCHAR col1[SQL_RESULT_LEN], col2[SQL_RESULT_LEN]; // Sütunların tipine göre değişebilir
        SQLLEN indicator1, indicator2;


        while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {

            SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, col1, SQL_RESULT_LEN, &indicator1);
            SQLGetData(sqlStmtHandle, 2, SQL_C_CHAR, col2, SQL_RESULT_LEN, &indicator2);

            if (indicator1 == SQL_NULL_DATA || indicator2 == SQL_NULL_DATA) {
                cout << "NULL" << endl;
            }
            else {
                cout << "Column 1: " << col1 << ", Column 2: " << col2 << endl;
            }
        }
    }

    //close connection and free resources
COMPLETED:

    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    //pause the console window - exit when key is pressed
    cout << "\nPress any key to exit...";
}
