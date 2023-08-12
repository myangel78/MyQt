// MyProtoBuf.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <string>
#include "person.pb.h"
using namespace std;
int main()
{
    std::cout << "Hello World!\n";
#if 0
    tutorial::Person person;
    person.set_id(30);
    person.set_name("John Doe");
    person.set_email("john.doe@example.com");

    // Serialize
    std::string data;
    person.SerializeToString(&data);

    // Deserialize
    tutorial::Person person2;
    person2.ParseFromString(data);
    std::cout << person2.id() << std::endl;
    std::cout << person2.name() << std::endl;
    std::cout << person2.email() << std::endl;
    system("pause");
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
#else
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    tutorial::Person person;

    //将数据写到person.pb文件
    person.set_id(123456);
    person.set_name("Mark");
    person.set_email("mark@example.com");

    fstream out("person.pb", ios::out | ios::binary | ios::trunc);
    person.SerializeToOstream(&out);
    out.close();


    //从person.pb文件读取数据
    fstream in("person.pb", ios::in | ios::binary);
    if (!person.ParseFromIstream(&in)) {
        cerr << "Failed to parse person.pb." << endl;
        exit(1);
    }

    cout << "ID: " << person.id() << endl;
    std::string s_name = person.name();
    cout << "name: " << person.name()  << endl;
    if (person.has_email()) {
        cout << "e-mail: " << person.email() << endl;
    }

    getchar();
    return 0;
#endif

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
