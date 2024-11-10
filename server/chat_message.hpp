/*//
// chat_message.hpp

// This header defines the chat_message class which is used to 
// represent chat messages in the chat application. The chat_message 
// class is used to encode and decode chat messages. The chat_message 
// class has a header_length of 4 bytes and a max_body_length of 512 bytes. The chat_message class has a data_ member which is an array of characters with a size of header_length + max_body_length. The chat_message class has a body_length_ member which is a size_t that represents the length of the body of the chat message. The chat_message class has a data() method that returns a const char* to the data_ member. The chat_message class has a data() method that returns a char* to the data_ member. The chat_message class has a length() method that returns the length of the chat message. The chat_message class has a body() method that returns a const char* to the body of the chat message. The chat_message class has a body() method that returns a char* to the body of the chat message. The chat_message class has a body_length() method that returns the length of the body of the chat message. The chat_message class has a body_length() method that sets the length of the body of the chat message. The chat_message class has a decode_header() method that decodes the header of the chat message. The chat_message class has an encode_header() method that encodes the header of the chat message.
#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

class chat_message
{
public:
    //to do
	// --> change the header_length to accomodate the sender_id and reciever_id
    static constexpr std::size_t header_length = 4;
    static constexpr std::size_t max_body_length = 512;

    chat_message()
        : body_length_(0)
    {
        std::memset(data_, 0, sizeof(data_));
    }

    const char* data() const
    {
        return data_;
    }

    char* data()
    {
        return data_;
    }

    std::size_t length() const
    {
        return header_length + body_length_;
    }

    const char* body() const
    {
		//returns pointer to the first character of the body
        return data_ + header_length;
    }

    char* body()
    {
        return data_ + header_length;
    }

	//returns the length of the body
    std::size_t body_length() const
    {
        return body_length_;
    }

	//sets the length of the body
    void body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }


    bool decode_header()
    {
		// defines header array and sets it to empty string
        char header[header_length + 1] = "";
		//copies the first 4 characters of data_ to header
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        // to do
		// --> seperate the header and set sender_id and reciever_id
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header()
    {

        char header[header_length + 1] = "";
		//it formats the header sting to be 4 characters long and stores 
        // body_length_ in it as a string
        std::sprintf(header, "%4d", static_cast<int>(body_length_));
        // to do
		// --> include sender_id and reciever_id in the header
        std::memcpy(data_, header, header_length);
    }

private:
    char data_[header_length + max_body_length];
    std::size_t body_length_;
	// each participant will have their unique id assigned to them to identify them
    double sender_id;
    double reciever_id;
};

class participant
{
    // to do
    // each participant have a unique id (double)
    // each participant have a unique name
	// returns the unique id of the participant
};

class sender : public participant
{
	// to do
    // returns the id of sender
};

class reciever : public participant
{
    // to do
    // returns the id of reciever
};

#endif // CHAT_MESSAGE_HPP

//g++ - o server server.cpp - I "C:\Users\Sanjay Shrestha\coded\cpp\asio\asio-1.30.2\include" - lws2_32 - lmswsock*/

//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

class chat_message
{
public:
    static constexpr std::size_t header_length = 24;
    static constexpr std::size_t max_body_length = 512;
    static constexpr int array_of_msg_id[] = { 1, 2, 3, 4, 5 };
    // 1 = login, 2 = logout, 3 = assigne_reciever, 4 = send_message, 5 = to be determined

    chat_message()
        : body_length_(0)
    {
    }

    const char* data() const
    {
        return data_;
    }

    char* data()
    {
        return data_;
    }

    std::size_t length() const
    {
        return header_length + body_length_;
    }

    const char* body() const
    {
        return data_ + header_length;
    }

    char* body()
    {
        return data_ + header_length;
    }

    std::size_t body_length() const
    {
        return body_length_;
    }

    void body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

    bool decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header(double sender_id, double reciever_id, int _msg_id)
    {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4d%4d%10.2f%10.2f", static_cast<int>(body_length_), _msg_id, sender_id, reciever_id);
        std::memcpy(data_, header, header_length);
    }

private:
    int msg_id;
    char data_[header_length + max_body_length];
    std::size_t body_length_;
};

#endif // CHAT_MESSAGE_HPP