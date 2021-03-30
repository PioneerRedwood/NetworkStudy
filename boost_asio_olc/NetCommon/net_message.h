#pragma once
#include "net_common.h"

namespace olc
{
	namespace net
	{
		// Message Header is sent at start of all messages. The template allows us
		// to use "enum class" to ensure that the messages are valid at compile time
		// enum Ŭ������ Ȱ��, ������ �ð� �� ��ȿ�� �޽����� �����ϱ� ���� ���ø�.
		template <typename T>
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		template <typename T>
		struct message 
		{
			message_header<T> header{};
			std::vector<uint8_t> body;

			// returns size of entire message packet in bytes
			// ��ƿ��Ƽ�� �޽��� ��� ũ��� �ٵ��� ũ�⸦ ���� ��ü �޽��� ũ�� ��ȯ
			size_t size() const
			{
				return body.size();
			}

			// Override for std::cout compatibility - produces friendly description of message
			// �޽����� ȿ��������(ģȭ������) ��Ƴ��� ���� << ������ �������̵�
			friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
			{
				os << "ID: " << int(msg.header.id) << " Size:" << msg.header.size;
				return os;
			}

			// Pushes any POD-like data into the message buffer
			template<typename DataType>
			friend message<T>& operator << (message<T>& msg, const DataType& data)
			{
				// Check that the type of the data being pushed is trivially copyable
				// �޽����� ���� ������ Ÿ�� �˻�, ������ ��� �����
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache currest size of vector, as this will be the point we insert the data 
				// �����͸� ��Ƴ��� ���� ���� ���� ��ġ ĳ��
				size_t i = msg.body.size();

				// Resize the vector by the size of the data being pushed
				// �޽��� ���� ũ�⸦ �� ������ Ÿ�Ը�ŭ ������
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// Physically copy the data into the newly allocated vector space
				// ũ�⸸ŭ �����ͷκ��� �о ���������� �޸� �Ҵ�
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				return msg;

				// �̷��� ������� ������ ������ ����
				// ����: �پ���(�⺻����) ������ Ÿ���� �޾Ƴ��� ũ�� ������, �޸� �Ҵ� ���� ó��
				// ����: �������� ����. �������� ũ�� �������� �߻� ����. �� �κ��� ���������� �������� ����� ������.
			}

			template <typename DataType>
			friend message<T>& operator >> (message<T>& msg, DataType& data)
			{
				// Check that the type of the data being pushed is trivially copyable
				// �޽����� ���� ������ Ÿ�� �˻�, ������ ��� �����
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache the location towards the end of the vector where the pulled data starts
				// ���Ϳ� �о���� ������ ���� ��ġ ĳ��
				size_t i = msg.body.size() - sizeof(DataType);

				// Physically copy the data from the vector into the user variable
				// ���� ������ ��Ƴ� �����͸� ���Ϳ� ����
				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				// Shrink the vector to remove read bytes, and reset and position
				msg.body.resize(i);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				// �ش� �޽��� ��ȯ, "ü�ε�"
				return msg;
			}
		};


		// Ŭ���̾�Ʈ�� ���� ���̿� �����ϴ� ���� Ŭ����(��� ��ü ����)
		// Forward declare the connection
		template <typename T>
		class connection;

		// ���� ������ Ŭ���̾�Ʈ�� �����ϱ� ���� ������ �޽��� ����ü
		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			// Again, a friendly string maker
			friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
		};
	}
}