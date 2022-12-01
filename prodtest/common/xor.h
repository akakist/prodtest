#ifndef ___XSOR__H
#define ___XSOR__H

#include <string>
inline std::string xor_encode_string(const std::string& buffer,const std::string& xor_passwd)
{
	std::string xored;
	for (size_t i = 0; i < buffer.size(); i++)
	{
		//char* p = xored.data();
                xored+=buffer[i] ^ xor_passwd[i%xor_passwd.size()];
	}
	return xored;
}

#endif
