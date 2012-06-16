#include "fs/IAttributes.h"
#include "fs/IEntry.h"
#include "fs/IEntryEnumerator.h"
#include <iostream>

class CB : public FS::IEntryEnumeratorCallBack
{
public:
	virtual FS::boolean callback(FS::IEntry& rEntry, FS::IAttributes& rAttributes)
	{
		std::cout<<(rAttributes.isDirectory()?"d":" ");
		std::cout<<(rAttributes.isFile()?"f":" ");
		std::cout<<(rAttributes.isSymbolicLink()?"s":" ");
		std::cout<<(" ");
		std::cout<<(rAttributes.isArchive()?"a":" ");
		std::cout<<(rAttributes.isReadOnly()?"r":" ");
		std::cout<<(rAttributes.isHidden()?"h":" ");
		std::cout<<(rAttributes.isSystem()?"s":" ");
		std::cout<<(rAttributes.isExecutable()?"x":" ");
		std::cout<<(" ");
		std::cout<<rEntry.getName();
		std::cout<<(" - ");
		std::cout<<rAttributes.getSize();
		std::cout<<std::endl;
		return true;
	}
};

int main(int argc, char** argv)
{
	std::cout<<"--"<<std::endl;

	CB cb;
	FS::IEntryEnumerator* l_pEnumerator=FS::createEntryEnumerator(cb);
	l_pEnumerator->enumerate(argv[1]);

	std::cout<<"--"<<std::endl;

	return 0;
}
