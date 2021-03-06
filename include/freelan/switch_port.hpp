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
 * \file switch_port.hpp
 * \author Julien KAUFFMANN <julien.kauffmann@freelan.org>
 * \brief A base switch port class.
 */

#ifndef SWITCH_PORT_HPP
#define SWITCH_PORT_HPP

#include <iostream>

#include <boost/asio.hpp>

namespace freelan
{
	/**
	 * \brief A base switch port class.
	 *
	 * Represent a switch port.
	 */
	class switch_port
	{
		public:

			/**
			 * \brief Virtual destructor.
			 */
			virtual ~switch_port();

		protected:

			/**
			 * \brief Send data trough the port.
			 * \param data The data to send trough the port.
			 */
			virtual void write(boost::asio::const_buffer data) = 0;

			/**
			 * \brief Check if the instance is equal to another.
			 * \param other The other instance to test for equality.
			 * \return true if the two instances are equal. Two instances of different subtypes are never equal.
			 */
			virtual bool equals(const switch_port& other) const = 0;

			/**
			 * \brief Output the name of the switch port to an output stream.
			 * \param os The output stream.
			 * \return os.
			 */
			virtual std::ostream& output(std::ostream& os) const = 0;

			friend class switch_;
			friend bool operator==(const switch_port&, const switch_port&);
			friend std::ostream& operator<<(std::ostream&, const switch_port&);
	};

	/**
	 * \brief Test two switch_port for equality.
	 * \param lhs The left argument.
	 * \param rhs The right argument.
	 * \return true if lhs and rhs have the exact same attributes.
	 */
	bool operator==(const switch_port& lhs, const switch_port& rhs);

	/**
	 * \brief Print a switch port to an output stream.
	 * \param os The output stream.
	 * \param port The switch port.
	 * \return os.
	 */
	std::ostream& operator<<(std::ostream& os, const switch_port& port);

	inline switch_port::~switch_port()
	{
	}

	inline bool operator==(const switch_port& lhs, const switch_port& rhs)
	{
		return lhs.equals(rhs);
	}

	inline std::ostream& operator<<(std::ostream& os, const switch_port& port)
	{
		return port.output(os);
	}
}

#endif /* SWITCH_PORT_HPP */

