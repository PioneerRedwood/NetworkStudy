#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
/*
namespace olc
{
	namespace net
	{
		template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:
			// "연결"은 서버와 클라이언트 양쪽 모두 소유됨
			// 행동은 조금씩 다름
			enum class owner
			{
				server,
				client
			};

		public:
			// 생성자: 특정 소유자, 맥락에 연결, 소켓에 전송
			//					들어오는 메시지 큐의 참조 제공
			connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
				: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
			{
				m_nOwnerType = parent;
			}

			virtual ~connection()
			{

			}

			// 클라이언트 별 구분을 위해 일반적으로 쓰이는 아이디, 두 시스템 모두 사용
			uint32_t GetID() const
			{
				return id;
			}

		public:
			void ConnectToClient(uint32_t uid = 0)
			{
				if (m_nOwnerType == owner::server)
				{
					if (m_socket.is_open())
					{
						id = uid;
						ReadHeader();
					}
				}
			}

			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (m_nOwnerType == owner::client)
				{
					asio::async_connect(m_socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								ReadHeader();
							}
						});
				}
			}

			void Disconnect()
			{
				if (IsConnected())
				{
					asio::post(m_asioContext, [this](){m_socket.close();});
				}
			}

			bool IsConnected() const
			{
				return m_socket.is_open();
			}

			void StartListening()
			{
				
			}

		public:
			// 연결은 1:1이므로 상대를 지정할 필요 없음
			void Send(const message<T>& msg)
			{
				asio::post(m_asioContext,
				[this, msg]()
				{
					// 대기열에 메시지가 있는 경우 비동기적으로 쓰는 중이라고 가정
					// 어느 쪽이든 출력할 큐에 메시지를 추가
					// 메시지가 없는 경우 쓸 수 있는 다음 프로세스(대기열 맨 앞 메시지) 진행
					bool bWrittenMessage = !m_qMessagesOut.empty();
					m_qMessagesOut.push_back(msg);

					if (!bWrittenMessage)
					{
						WriteHeader();
					}
				});
			}

		private:
			// 비동기 - Prime context에 메시지 헤더 작성
			void WriteHeader()
			{
				// 해당 함수가 호출된 것은 바깥으로 보내야 할 메시지가 적어도 한 개는 있는 것
				// 버퍼에 할당한 뒤 전송, 처리
				asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_qMessagesOut.front().body.size() > 0)
							{
								WriteBody();
							}
							else
							{
								m_qMessagesOut.pop_front();

								if (!m_qMessagesOut.empty())
								{
									WriteHeader();
								}
							}
						}
						else
						{
							std::cout << "[" << id << "] Write header fail.\n";
							m_socket.close();
						}
					});
			}

			// 비동기 - Prime context에 메시지 바디 작성
			void WriteBody()
			{
				asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							m_qMessagesOut.pop_front();

							if (m_qMessagesOut.empty())
							{
								WriteHeader();
							}
						}
						else
						{
							std::cout << "[" << id << "] Write body fail.\n";
							m_socket.close();
						}
					});
			}

			// 비동기 - Prime context 메시지 헤더 읽기
			void ReadHeader()
			{
				asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_msgTemporaryIn.header.size > 0)
							{
								m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
								ReadBody();
							}
							else
							{
								AddToIncomingMessageQueue();
							}
						}
						else
						{
							std::cout << "[" << id << "] Read header fail.\n";
							m_socket.close();
						}
					});
			}

			// 비동기 - Prime context 메시지 바디 읽기
			void ReadBody()
			{
				asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							AddToIncomingMessageQueue();
						}
						else
						{
							std::cout << "[" << id << "] Read body fail.\n";
							m_socket.close();
						}
					});
			}

			void AddToIncomingMessageQueue()
			{
				if (m_nOwnerType == owner::server)
				{
					m_qMessagesIn.push_back({this->shared_from_this(), m_msgTemporaryIn});
				}
				else
				{
					m_qMessagesIn.push_back({nullptr, m_msgTemporaryIn});
				}

				ReadHeader();
			}

		protected:
			asio::ip::tcp::socket m_socket;

			asio::io_context& m_asioContext;

			tsqueue<message<T>> m_qMessagesOut;

			tsqueue<owned_message<T>> m_qMessagesIn;

			message<T> m_msgTemporaryIn;

			owner m_nOwnerType = owner::server;

			uint32_t id = 0;
		};
	}
}
*/
namespace olc
{
	namespace net
	{
		template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:
			enum class owner
			{
				server,
				client
			};

		public:
			connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
				: m_context(asioContext), m_socket(std::move(socket)), m_recvDeq(qIn)
			{
				m_ownerType = parent;
			}

			virtual ~connection()
			{}

			uint32_t GetID() const
			{
				return id;
			}

		public:
			// 서버에서만 호출
			void ConnectToClient(uint32_t uid = 0)
			{
				if (m_ownerType == owner::server)
				{
					if (m_socket.is_open())
					{
						id = uid;
						ReadHeader();
					}
				}
			}

			// 클라이언트에서만 호출
			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (m_ownerType == owner::client)
				{
					asio::async_connect(m_socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								ReadHeader();
							}
						});
				}
			}


			void Disconnect()
			{
				if (IsConnected())
					asio::post(m_context, [this]() { m_socket.close(); });
			}

			bool IsConnected() const
			{
				return m_socket.is_open();
			}

		public:
			void Send(const message<T>& msg)
			{
				// 이 작업이 바로 전송 덱에 메시지 담는 행위
				// 성공적으로 담으면 헤더 작성하러 ㄱㄱ
				asio::post(m_context,
					[this, msg]()
					{
						bool bWritingMessage = !m_sendDeq.empty();
						m_sendDeq.push_back(msg);
						if (!bWritingMessage)
						{
							WriteHeader();
						}
					});
			}



		private:
			// 비동기 헤더 작성
			void WriteHeader()
			{
				// 비동기 쓰기
				// 전송 덱 선두에 있는 메시지 헤더를 버퍼에 쓰기
				asio::async_write(m_socket, asio::buffer(&m_sendDeq.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_sendDeq.front().body.size() > 0)
							{
								WriteBody();
							}
							else
							{
								m_sendDeq.pop_front();

								if (!m_sendDeq.empty())
								{
									WriteHeader();
								}
							}
						}
						else
						{
							std::cout << "[" << id << "] Write Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// 비동기 바디 작성
			void WriteBody()
			{
				// 비동기 쓰기
				// 전송 덱 선두에 있는 메시지 바디를 버퍼에 쓰기
				asio::async_write(m_socket, asio::buffer(m_sendDeq.front().body.data(), m_sendDeq.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							m_sendDeq.pop_front();

							if (!m_sendDeq.empty())
							{
								WriteHeader();
							}
						}
						else
						{
							std::cout << "[" << id << "] Write Body Fail.\n";
							m_socket.close();
						}
					});
			}

			// 비동기 헤더 읽기
			void ReadHeader()
			{
				// 비동기 읽기
				// 받은 임시 메시지 헤더를 버퍼로 읽어오기
				asio::async_read(m_socket, asio::buffer(&m_tempMsg.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// 헤더 크기가 있다는건 정상적인 메시지라는 의미로 바디를 읽을 차례!
							if (m_tempMsg.header.size > 0)
							{
								// 그 전에 벡터 크기 재조정
								m_tempMsg.body.resize(m_tempMsg.header.size);
								ReadBody();
							}
							else
							{
								AddToIncomingMessageQueue();
							}
						}
						else
						{
							std::cout << "[" << id << "] Read Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// 비동기 바디 읽기
			void ReadBody()
			{
				// 비동기 읽기
				// 받은 임시 메시지 바디를 버퍼로 읽어오기
				asio::async_read(m_socket, asio::buffer(m_tempMsg.body.data(), m_tempMsg.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// 마지막으로 대기열에 추가
							AddToIncomingMessageQueue();
						}
						else
						{
							std::cout << "[" << id << "] Read Body Fail.\n";
							m_socket.close();
						}
					});
			}

			void AddToIncomingMessageQueue()
			{
				if (m_ownerType == owner::server)
				{
					m_recvDeq.push_back({ this->shared_from_this(), m_tempMsg });
				}
				else
				{
					m_recvDeq.push_back({ nullptr, m_tempMsg });
				}

				ReadHeader();
			}

		protected:
			asio::ip::tcp::socket m_socket;

			asio::io_context& m_context;

			tsqueue<message<T>> m_sendDeq;

			tsqueue<owned_message<T>>& m_recvDeq;

			message<T> m_tempMsg;

			owner m_ownerType = owner::server;

			uint32_t id = 0;

		};
	}
}