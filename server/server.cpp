
// chat_server.cpp


#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "chat_message.hpp"

using asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------
//creates a virtual class that represents the chat participant
class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------
//creates a class that represents the chat room where participants can join and leave
class chat_room
{
public:
    void join(chat_participant_ptr participant)
    {
        participants_.insert(participant);
        for (auto msg : recent_msgs_)
            participant->deliver(msg);
    }

    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
    }

    void deliver(const chat_message& msg)
    {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        for (auto participant : participants_)
            participant->deliver(msg);
    }

private:
    std::set<chat_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------
//creates a class that represents the chat session where participants can start and deliver messages
class chat_session
    : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
private:
    tcp::socket socket_;
    chat_room& room_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;

    void do_read_header()
    {
        // This line creates a std::shared_ptr to the 
        // current chat_session object using shared_from_this(). 
        // This ensures that the chat_session object remains alive 
        // for the duration of the asynchronous operation.
        auto self(shared_from_this());
        //It reads chat_message::header_length bytes into the 
        // data() buffer of read_msg_.
        asio::async_read(socket_,
            asio::buffer(read_msg_.data(), chat_message::header_length),
            [this, self](std::error_code ec, std::size_t /*length*/)
            {
                if (!ec && read_msg_.decode_header())
                {
                    do_read_body();
                }
                else
                {
                    room_.leave(shared_from_this());
                }
            });
    }

    void do_read_body()
    {
        auto self(shared_from_this());
        asio::async_read(socket_,
            asio::buffer(read_msg_.body(), read_msg_.body_length()),
            [this, self](std::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    room_.deliver(read_msg_);
                    do_read_header();
                }
                else
                {
                    room_.leave(shared_from_this());
                }
            });
    }

    void do_write()
    {
        auto self(shared_from_this());
        asio::async_write(socket_,
            asio::buffer(write_msgs_.front().data(),
                write_msgs_.front().length()),
            [this, self](std::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty())
                    {
                        do_write();
                    }
                }
                else
                {
                    room_.leave(shared_from_this());
                }
            });
    }


public:
	//initialization of the chat session
    chat_session(tcp::socket socket, chat_room& room)
        : socket_(std::move(socket)),
        room_(room)
    {
    }

	//starts the chat session
    void start()
    {
        room_.join(shared_from_this());
        do_read_header();
    }

	//delivers the message
    void deliver(const chat_message& msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            do_write();
        }
    }


};

//----------------------------------------------------------------------

class chat_server
{
public:
    chat_server(asio::io_context& io_context,
        const tcp::endpoint& endpoint)
        : acceptor_(io_context, endpoint)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<chat_session>(std::move(socket), room_)->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        asio::io_context io_context;

        std::list<chat_server> servers;
        for (int i = 1; i < argc; ++i)
        {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_context, endpoint);
        }

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}