#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };  

int main(int argc, char* argv[], int)
{
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;  
        const char* host = argv[1];                // Получаем хост из аргументов командной строки
        const char* port = argv[2];                // Получаем порт
        int number_clients = std::atoi(argv[3]);
        std::cout << "Enter number of cycles: ";
        int num_cycles = 0;
        std::cin >> num_cycles;
        std::string wsp;
        std::getline(std::cin, wsp);

        for (int i = 0; i < number_clients; i++) {
            
            tcp::socket s(io_context);  
            tcp::resolver resolver(io_context);  
            
            boost::asio::connect(s, resolver.resolve(host, port));

            for (int j = 0; j <= num_cycles; j++) {
                if (j == 0) {
                    std::cout << std::endl << "The client is connected" << std::endl; 
                }
                else {
                    std::cout << "Enter message: ";
                    char request[max_length];
                    std::cin.getline(request, max_length);  // Получаем сообщение от пользователя и сохраняем в буфер
                    size_t request_length = std::strlen(request); 

                    // Отправляем сообщение на сервер
                    boost::asio::write(s, boost::asio::buffer(request, request_length));

                    char reply[max_length];
                    size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, request_length));  // Ожидаем ответ от сервера
                    std::cout << "Reply is: ";
                    std::cout.write(reply, reply_length);
                    std::cout << "\n";
                }
            }
            std::cout << "The client is disconnected" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n"; 
    }

    return 0;
}