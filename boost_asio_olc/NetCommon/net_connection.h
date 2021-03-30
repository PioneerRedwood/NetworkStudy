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
			// "����"�� ������ Ŭ���̾�Ʈ ���� ��� ������
			// �ൿ�� ���ݾ� �ٸ�
			enum class owner
			{
				server,
				client
			};

		public:
			// ������: Ư�� ������, �ƶ��� ����, ���Ͽ� ����
			//					������ �޽��� ť�� ���� ����
			connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
				: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
			{
				m_nOwnerType = parent;
			}

			virtual ~connection()
			{

			}

			// Ŭ���̾�Ʈ �� ������ ���� �Ϲ������� ���̴� ���̵�, �� �ý��� ��� ���
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
			// ������ 1:1�̹Ƿ� ��븦 ������ �ʿ� ����
			void Send(const message<T>& msg)
			{
				asio::post(m_asioContext,
				[this, msg]()
				{
					// ��⿭�� �޽����� �ִ� ��� �񵿱������� ���� ���̶�� ����
					// ��� ���̵� ����� ť�� �޽����� �߰�
					// �޽����� ���� ��� �� �� �ִ� ���� ���μ���(��⿭ �� �� �޽���) ����
					bool bWrittenMessage = !m_qMessagesOut.empty();
					m_qMessagesOut.push_back(msg);

					if (!bWrittenMessage)
					{
						WriteHeader();
					}
				});
			}

		private:
			// �񵿱� - Prime context�� �޽��� ��� �ۼ�
			void WriteHeader()
			{
				// �ش� �Լ��� ȣ��� ���� �ٱ����� ������ �� �޽����� ��� �� ���� �ִ� ��
				// ���ۿ� �Ҵ��� �� ����, ó��
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

			// �񵿱� - Prime context�� �޽��� �ٵ� �ۼ�
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

			// �񵿱� - Prime context �޽��� ��� �б�
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

			// �񵿱� - Prime context �޽��� �ٵ� �б�
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
			// ���������� ȣ��
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

			// Ŭ���̾�Ʈ������ ȣ��
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
				// �� �۾��� �ٷ� ���� ���� �޽��� ��� ����
				// ���������� ������ ��� �ۼ��Ϸ� ����
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
			// �񵿱� ��� �ۼ�
			void WriteHeader()
			{
				// �񵿱� ����
				// ���� �� ���ο� �ִ� �޽��� ����� ���ۿ� ����
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

			// �񵿱� �ٵ� �ۼ�
			void WriteBody()
			{
				// �񵿱� ����
				// ���� �� ���ο� �ִ� �޽��� �ٵ� ���ۿ� ����
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

			// �񵿱� ��� �б�
			void ReadHeader()
			{
				// �񵿱� �б�
				// ���� �ӽ� �޽��� ����� ���۷� �о����
				asio::async_read(m_socket, asio::buffer(&m_tempMsg.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// ��� ũ�Ⱑ �ִٴ°� �������� �޽������ �ǹ̷� �ٵ� ���� ����!
							if (m_tempMsg.header.size > 0)
							{
								// �� ���� ���� ũ�� ������
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

			// �񵿱� �ٵ� �б�
			void ReadBody()
			{
				// �񵿱� �б�
				// ���� �ӽ� �޽��� �ٵ� ���۷� �о����
				asio::async_read(m_socket, asio::buffer(m_tempMsg.body.data(), m_tempMsg.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// ���������� ��⿭�� �߰�
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