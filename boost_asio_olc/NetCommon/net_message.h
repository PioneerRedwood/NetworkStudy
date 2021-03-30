#pragma once
#include "net_common.h"

namespace olc
{
	namespace net
	{
		// Message Header is sent at start of all messages. The template allows us
		// to use "enum class" to ensure that the messages are valid at compile time
		// enum 클래스를 활용, 컴파일 시간 내 유효한 메시지를 보장하기 위한 템플릿.
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
			// 유틸리티로 메시지 헤더 크기와 바디의 크기를 더한 전체 메시지 크기 반환
			size_t size() const
			{
				return body.size();
			}

			// Override for std::cout compatibility - produces friendly description of message
			// 메시지를 효과적으로(친화적으로) 담아냄기 위한 << 연산자 오버라이딩
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
				// 메시지에 담을 데이터 타입 검사, 복잡할 경우 경고함
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache currest size of vector, as this will be the point we insert the data 
				// 데이터를 담아내기 위해 현재 벡터 위치 캐시
				size_t i = msg.body.size();

				// Resize the vector by the size of the data being pushed
				// 메시지 벡터 크기를 들어갈 데이터 타입만큼 재조정
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// Physically copy the data into the newly allocated vector space
				// 크기만큼 데이터로부터 읽어서 물리적으로 메모리 할당
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				return msg;

				// 이러한 방법에는 장점과 단점이 존재
				// 장점: 다양한(기본적인) 데이터 타입을 받아내고 크기 재조정, 메모리 할당 등을 처리
				// 단점: 성능적인 문제. 지속적인 크기 재조정이 발생 가능. 이 부분은 현실적으로 선형적인 결과를 보여줌.
			}

			template <typename DataType>
			friend message<T>& operator >> (message<T>& msg, DataType& data)
			{
				// Check that the type of the data being pushed is trivially copyable
				// 메시지에 담을 데이터 타입 검사, 복잡할 경우 경고함
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache the location towards the end of the vector where the pulled data starts
				// 벡터에 밀어넣을 데이터 저장 위치 캐시
				size_t i = msg.body.size() - sizeof(DataType);

				// Physically copy the data from the vector into the user variable
				// 유저 변수를 담아낼 데이터를 벡터에 저장
				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				// Shrink the vector to remove read bytes, and reset and position
				msg.body.resize(i);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be "chained"
				// 해당 메시지 반환, "체인됨"
				return msg;
			}
		};


		// 클라이언트와 서버 사이에 존재하는 연결 클래스(통신 객체 역할)
		// Forward declare the connection
		template <typename T>
		class connection;

		// 서버 측에서 클라이언트를 구분하기 위한 소유된 메시지 구조체
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