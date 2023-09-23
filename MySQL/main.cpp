#include <iostream> 
#include <string>
#include <mysql/mysql.h>
using namespace std;

const string host = "localhost";
const string user = "Fjl";
const string passwd = "011210Fjl";
const string DB = "Fjl";
const string table = "users";
const unsigned int port = 3306;
int main()
{
    // cout << "version: " << mysql_get_client_info() << endl;
    MYSQL* my_sql = mysql_init(nullptr);
    if(!my_sql)
    {
        cerr << "init mysql error" << endl;
        return 1;
    }
    cout << "init success" << endl;
    if(mysql_real_connect(my_sql, host.c_str(), user.c_str(), passwd.c_str(), DB.c_str(), port, nullptr, 0) == nullptr)
    {
        cerr << "connect error" << endl;
        return 2;
    }
    cout << "connect success" << endl;
    if(mysql_set_character_set(my_sql, "utf8"))
    {
        cerr << "charset error" << endl;
    }
    // mysql_query(my_sql, "insert into users (name, telephone) values('mark', '1234567890')");
    // while(true)
    // {
    //     string sql;
    //     cout << "please enter: ";
    //     if(!getline(cin, sql) || sql == "quit")
    //     {
    //         cout << "bye~" << endl;
    //         break;
    //     }
    //     if(mysql_query(my_sql, sql.c_str()))
    //     {
    //         cout << "sql error" << endl;
    //     }
    //     else
    //     {
    //         cout << "sql success" << endl;
    //     }
    // }
    const string sql = "select * from users";
    mysql_query(my_sql, sql.c_str());
    MYSQL_RES* res = mysql_store_result(my_sql);
    cout << "行: " << res->row_count<< endl;
    cout << "列: " << res->field_count << endl;
    int fields = mysql_num_fields(res); //获取列数量
    MYSQL_FIELD* field = mysql_fetch_field(res);
    for(int i = 0; i < fields; ++i)
    {
        cout << field[i].name << "\t";
    }
    cout << endl;
    //获取行信息
    for(int i = 0; i < res->row_count; ++i)
    {
        MYSQL_ROW lines = mysql_fetch_row(res);
        for(int j = 0; j < res->field_count; ++j)
        {
            cout << lines[j] << '\t';
        }
        cout << endl;
    }
    mysql_free_result(res);
    mysql_close(my_sql);   
    return 0;
}