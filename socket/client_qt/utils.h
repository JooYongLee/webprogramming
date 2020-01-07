#ifndef TCPCLIENT_UTILS_H
#define TCPCLIENT_UTILS_H

#include <string>

template <class Container>
void str_split(const std::string& str, Container& cont,
	char delim = ' ')
{
	std::size_t current, previous = 0;
	current = str.find(delim);
	while (current != std::string::npos) {
		cont.push_back(str.substr(previous, current - previous));
		previous = current + 1;
		current = str.find(delim, previous);
	}
	cont.push_back(str.substr(previous, current - previous));
}

#endif // TCPCLIENT_UTILS_H

