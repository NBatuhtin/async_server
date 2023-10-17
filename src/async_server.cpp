#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, int id)
        : socket_(std::move(socket)), id_(id)
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this()); // Создаем shared_ptr для предотвращения удаления объекта во время асинхронной операции
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    do_write(length); 
                }
                else
                {
                    client_disconnect(ec);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this()); 
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read(); 
                }
                else
                {
                    client_disconnect(ec);
                }
            });
    }

    void client_disconnect(const boost::system::error_code& ec)
    {
        std::cout << "Connection lost: " << id_ << std::endl;
    }

    tcp::socket socket_;
    int id_; // Уникальный id клиента
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), id_counter_(1)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket), id_counter_++)->start(); // Создаем нового клиента с уникальным id
                    std::cout << "Connection established: " << id_counter_ - 1 << "\n"; // Выводим информацию о подключении клиента
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    int id_counter_; 
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run(); 
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}