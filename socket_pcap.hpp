/*
utrack is a very small an efficient BitTorrent tracker
Copyright (C) 2013-2014 Arvid Norberg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <pcap/pcap.h>
#include <array>

enum { buffer_size = 16384 };

struct packet_socket
{
	explicit packet_socket(bool receive = false);
	~packet_socket();
	packet_socket(packet_socket&& s);
	packet_socket(packet_socket const&) = delete;

	void close();

	bool send(iovec const* v, int num, sockaddr const* to, socklen_t tolen);

	// fills in the in_packets array with incoming packets. Returns the number filled in
	int receive(incoming_packet_t* in_packets, int num);

private:

	void drain_send_queue();

	pcap_t* m_pcap;
	int m_link_layer;
	std::atomic<uint32_t> m_closed;
	std::array<uint64_t, buffer_size> m_buffer;

	// this mutex just protects the send buffer
	std::mutex m_mutex;

	// this contains all packets we want to send as one contiguous array. Each
	// packet has a 2 byte, host order length prefix, followed by that many
	// bytes of payload
	std::array<uint8_t, 0x100000> m_send_buffer;
	int m_send_cursor;
};

