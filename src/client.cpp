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
 * \file client.cpp
 * \author Julien KAUFFMANN <julien.kauffmann@freelan.org>
 * \brief A client implementation.
 */

#include "client.hpp"

#include <cassert>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "configuration.hpp"
#include "logger.hpp"
#include "logger_stream.hpp"
#include "curl.hpp"

namespace freelan
{
	namespace
	{
		std::string server_protocol_to_scheme(const server_configuration::server_protocol_type& protocol)
		{
			switch (protocol)
			{
				case server_configuration::SP_HTTP:
					return "http://";
				case server_configuration::SP_HTTPS:
					return "https://";
			}

			throw std::runtime_error("Unsupported server protocol.");
		}

		bool has_value(const client::values_type& values, const std::string& key, std::string& value)
		{
			client::values_type::const_iterator it = values.find(key);
		 
			if (it!= values.end())
			{
				value = it->second;

				return true;
			}

			return false;
		}

		void assert_has_value(const client::values_type& values, const std::string& key, std::string& value)
		{
			if (!has_value(values, key, value))
			{
				throw std::runtime_error("Missing required value \"" + key + "\".");
			}
		}

		template <typename T>
		void assert_has_value(const client::values_type& values, const std::string& key, T& value)
		{
			std::string str_value;

			assert_has_value(values, key, str_value);

			value = boost::lexical_cast<T>(str_value);
		}

		void json_to_values(const std::string& json, client::values_type& values)
		{
			values.clear();

			rapidjson::Document document;

			document.Parse<0>(json.c_str());

			if (document.HasParseError())
			{
				throw std::runtime_error("JSON syntax parse error.");
			}

			if (!document.IsObject())
			{
				throw std::runtime_error("JSON document parse error: root must be an object.");
			}

			for (rapidjson::Document::ConstMemberIterator it = document.MemberBegin(); it != document.MemberEnd(); ++it)
			{
				const char* name = it->name.GetString();

				if (!it->value.IsString())
				{
					throw std::runtime_error("JSON document parse error: values must be strings (" + std::string(name) + ").");
				}

				const char* value = it->value.GetString();

				values[name] = value;
			}
		}

		std::string values_to_json(const client::values_type& values)
		{
			rapidjson::StringBuffer strbuf;

			rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

			writer.StartObject();

			for (client::values_type::const_iterator it = values.begin(); it != values.end(); ++it)
			{
				writer.String(it->first.c_str(), it->first.size());
				writer.String(it->second.c_str(), it->second.size());
			}

			writer.EndObject();

			return strbuf.GetString();
		}
	}

	client::client(freelan::configuration& configuration, freelan::logger& _logger) :
		m_configuration(configuration),
		m_logger(_logger),
		m_scheme(server_protocol_to_scheme(m_configuration.server.protocol))
	{
	}

	void client::connect()
	{
		curl request;

		configure_request(request);

		std::string server_name;
		unsigned int server_version_major;
		unsigned int server_version_minor;

		get_server_information(request, server_name, server_version_major, server_version_minor);
	}
	
	void client::configure_request(curl& request)
	{
		// Set the timeout
		request.set_connect_timeout(boost::posix_time::seconds(5));

		// Set the user agent
		if (m_configuration.server.user_agent.empty())
		{
			m_logger(LL_WARNING) << "Empty user agent specified, taking libcurl's default.";
		}
		else
		{
			m_logger(LL_INFORMATION) << "User agent set to \"" << m_configuration.server.user_agent << "\".";

			request.set_user_agent(m_configuration.server.user_agent);
		}

		// Set the HTTP proxy
		if (m_configuration.server.https_proxy)
		{
			if (*m_configuration.server.https_proxy != hostname_endpoint::null())
			{
				m_logger(LL_INFORMATION) << "Setting HTTP(S) proxy to \"" << *m_configuration.server.https_proxy << "\".";
			}
			else
			{
				m_logger(LL_INFORMATION) << "Disabling HTTP(S) proxy.";
			}

			request.set_proxy(*m_configuration.server.https_proxy);
		}

		// Disable peer verification if required
		if (m_configuration.server.disable_peer_verification)
		{
			m_logger(LL_WARNING) << "Peer verification disabled ! Connection will be a LOT LESS SECURE.";

			request.set_ssl_peer_verification(false);
		}
		else
		{
			if (!m_configuration.server.ca_info.empty())
			{
				m_logger(LL_INFORMATION) << "Setting CA info to \"" << m_configuration.server.ca_info.string() << "\"";

				request.set_ca_info(m_configuration.server.ca_info);
			}
		}

		// Disable host verification if required
		if (m_configuration.server.disable_host_verification)
		{
			m_logger(LL_WARNING) << "Host verification disabled ! Connection will be less secure.";

			request.set_ssl_host_verification(false);
		}

		// Set the read callback
		request.set_write_function(boost::bind(&client::read_data, this, _1));
	}

	void client::perform_request(curl& request, const std::string& url, values_type& values)
	{
		request.set_url(url);

		m_data.clear();

		request.perform();

		const long response_code = request.get_response_code();

		m_logger(LL_DEBUG) << "HTTP response code: " << response_code;
		m_logger(LL_DEBUG) << "Sent: GET " << url;
		m_logger(LL_DEBUG) << "Received:\n" << m_data;

		if (response_code != 200)
		{
			m_logger(LL_ERROR) << "Unexpected HTTP response code " << response_code << ".";

			throw std::runtime_error("HTTP request failed.");
		}
		else
		{
			const std::string content_type = request.get_content_type();

			if (content_type != "application/json")
			{
				m_logger(LL_ERROR) << "Unsupported content type received: " << content_type;

				throw std::runtime_error("Unexpected server error.");
			}
			else
			{
				json_to_values(m_data, values);
			}
		}
	}

	void client::perform_get_request(curl& request, const std::string& url, values_type& values)
	{
		request.set_get();

		request.set_http_header("Accept", "application/json");

		perform_request(request, url, values);
	}

	void client::perform_post_request(curl& request, const std::string& url, const values_type& parameters, values_type& values)
	{
		request.set_post();

		request.set_http_header("Accept", "application/json");
		request.set_http_header("Content-Type", "application/json");
		request.unset_http_header("Expect");

		request.set_copy_post_fields(boost::asio::buffer(values_to_json(parameters)));

		perform_request(request, url, values);
	}

	void client::get_server_information(
			curl& request,
			std::string& server_name,
			unsigned int& server_version_major,
			unsigned int& server_version_minor
			)
	{
		m_logger(LL_INFORMATION) << "Getting server information from " << m_configuration.server.host << "...";

		const std::string url = m_scheme + boost::lexical_cast<std::string>(m_configuration.server.host) + "/api/information";

		request.reset_http_headers();

		values_type values;

		perform_get_request(request, url, values);

		assert_has_value(values, "name", server_name);
		assert_has_value(values, "major", server_version_major);
		assert_has_value(values, "minor", server_version_minor);

		m_logger(LL_INFORMATION) << "Server version is " << server_name << "/" << server_version_major << "." << server_version_minor;
	}

	size_t client::read_data(boost::asio::const_buffer buf)
	{
		const char* _data = boost::asio::buffer_cast<const char*>(buf);
		size_t data_len = boost::asio::buffer_size(buf);

		m_data.append(_data, data_len);

		return data_len;
	}
}