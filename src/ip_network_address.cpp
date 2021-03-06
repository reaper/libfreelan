/*
 * libfreelan - A C++ library to establish peer-to-peer virtual private
 * networks.
 * Copyright (C) 2010-2011 Julien KAUFFMANN <julien.kauffmann@freelan.org>
 *
 * This file is part of libfreelan.
 *
 * libfreelan is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * libfreelan is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 *
 * If you intend to use libfreelan in a commercial software, please
 * contact me : we may arrange this for a small fee or no fee at all,
 * depending on the nature of your project.
 */

/**
 * \file ip_network_address.cpp
 * \author Julien KAUFFMANN <julien.kauffmann@freelan.org>
 * \brief IP network address classes.
 */

#include "ip_network_address.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "stream_operations.hpp"

namespace freelan
{
	namespace
	{
		template <typename AddressType>
		std::istream& read_ip_address_prefix_length(std::istream& is, std::string& ip_address, std::string& prefix_length)
		{
			if (is.good())
			{
				if (read_ip_address<AddressType>(is, ip_address))
				{
					if (is.good() && (is.peek() == '/'))
					{
						is.ignore();

						if (!read_prefix_length<AddressType>(is, prefix_length))
						{
							putback(is, ip_address + '/');
							is.setstate(std::ios_base::failbit);
						}
					}
					else
					{
						putback(is, ip_address);
						is.setstate(std::ios_base::failbit);
					}
				}
			}

			return is;
		}
	}

	template <typename AddressType>
	bool base_ip_network_address<AddressType>::has_address(const AddressType& addr) const
	{
		const typename AddressType::bytes_type network_bytes = address().to_bytes();
		const typename AddressType::bytes_type addr_bytes = addr.to_bytes();

		unsigned int prefix_len = m_prefix_length;

		for (size_t i = 0; i < network_bytes.size(); ++i)
		{
			if (prefix_len >= 8)
			{
				if (network_bytes[i] != addr_bytes[i])
					return false;

				prefix_len -= 8;
			}
			else
			{
				const typename AddressType::bytes_type::value_type mask = (static_cast<unsigned char>(0xFF) >> prefix_len) ^ static_cast<unsigned char>(0xFF);

				if ((network_bytes[i] & mask) != (addr_bytes[i] & mask))
					return false;

				break;
			}
		}

		return true;
	}

	template bool ipv4_network_address::has_address(const boost::asio::ip::address_v4&) const;
	template bool ipv6_network_address::has_address(const boost::asio::ip::address_v6&) const;

	template <typename AddressType>
	std::istream& operator>>(std::istream& is, base_ip_network_address<AddressType>& value)
	{
		std::string ip_address;
		std::string prefix_length;

		if (read_ip_address_prefix_length<AddressType>(is, ip_address, prefix_length))
		{
			value = base_ip_network_address<AddressType>(AddressType::from_string(ip_address), boost::lexical_cast<unsigned int>(prefix_length));
		}

		return is;
	}

	template std::istream& operator>>(std::istream& is, ipv4_network_address& value);
	template std::istream& operator>>(std::istream& is, ipv6_network_address& value);

	template <typename AddressType>
	std::ostream& operator<<(std::ostream& os, const base_ip_network_address<AddressType>& value)
	{
		return os << value.address().to_string() << "/" << std::dec << value.prefix_length();
	}

	template std::ostream& operator<<(std::ostream& is, const ipv4_network_address& value);
	template std::ostream& operator<<(std::ostream& is, const ipv6_network_address& value);

	std::istream& operator>>(std::istream& is, ip_network_address& value)
	{
		if (is)
		{
			ipv6_network_address ina;

			if (is >> ina)
			{
				value = ina;
				return is;
			}

			is.clear();
		}

		if (is)
		{
			ipv4_network_address ina;

			if (is >> ina)
			{
				value = ina;
				return is;
			}

			is.clear();
		}

		return is;
	}
}
