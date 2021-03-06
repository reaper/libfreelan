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
 * \file stream_operations.hpp
 * \author Julien KAUFFMANN <julien.kauffmann@freelan.org>
 * \brief Stream related functions.
 */

#ifndef FREELAN_STREAM_OPERATIONS_HPP
#define FREELAN_STREAM_OPERATIONS_HPP

#include <iostream>
#include <string>

namespace freelan
{
	/**
	 * \brief Put back some characters in an input stream.
	 * \param is The input stream.
	 * \param str The characters to put back.
	 * \return is.
	 */
	std::istream& putback(std::istream& is, const std::string& str);

	/**
	 * \brief Read an IP address.
	 * \tparam AddressType The address type.
	 * \param is The input stream.
	 * \param ip_address The result address.
	 * \return is.
	 */
	template <typename AddressType>
	std::istream& read_ip_address(std::istream& is, std::string& ip_address);

	/**
	 * \brief Read a port number.
	 * \param is The input stream.
	 * \param port The result port.
	 * \return is.
	 */
	std::istream& read_port(std::istream& is, std::string& port);

	/**
	 * \brief Read a prefix length.
	 * \tparam AddressType The address type.
	 * \param is The input stream.
	 * \param prefix_length The result prefix_length.
	 * \return is.
	 */
	template <typename AddressType>
	std::istream& read_prefix_length(std::istream& is, std::string& prefix_length);

	/**
	 * \brief Read a hostname.
	 * \param is The input stream.
	 * \param hostname The result hostname.
	 * \return is.
	 */
	std::istream& read_hostname(std::istream& is, std::string& hostname);

	/**
	 * \brief Read a service string.
	 * \param is The input stream.
	 * \param service The result service.
	 * \return is.
	 */
	std::istream& read_service(std::istream& is, std::string& service);
}

#endif /* FREELAN_STREAM_OPERATIONS_HPP */

