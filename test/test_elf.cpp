#include <iostream>
#include <elfio/elfio.hpp>

using namespace ELFIO;

int main(int argc, char** argv){

    if(argc != 2) {
        std::cout << "Usage: tutorial <elf_file>" << std::endl;
        return 0;
    }

    elfio reader;

    if(!reader.load(argv[1]))
    {
        std::cout << "Can't find or process ELF file" << argv[1] << std::endl;
        return 1;
    }

    std::cout << "ELF file class :";

    if(reader.get_class() == ELFCLASS32)
        std::cout << "ELF32" << std::endl;
    else
        std::cout << "ELF64" << std::endl;

    std::cout << "ELF file encoding : ";
    if ( reader.get_encoding() == ELFDATA2LSB )
        std::cout << "Little endian" << std::endl;
    else
        std::cout << "Big endian" << std::endl;


    // Print ELF file sections info
    Elf_Half sec_num = reader.sections.size();
    std::cout << "Number of sections: " << sec_num << std::endl;
    for ( int i = 0; i < sec_num; ++i ) {
        section* psec = reader.sections[i];
        std::cout << "  [" << i << "] "
                  << psec->get_name()
                  << "\t"
                  << psec->get_size()
                  << std::endl;
        // Access to section's data
        // const char* p = reader.sections[i]->get_data()
    }

    // Print ELF file segments info
    Elf_Half seg_num = reader.segments.size();
    std::cout << "Number of segments: " << seg_num << std::endl;
    for ( int i = 0; i < seg_num; ++i ) {
        const segment* pseg = reader.segments[i];
        std::cout << "  [" << i << "] 0x" << std::hex
                  << pseg->get_flags()
                  << "\t0x"
                  << pseg->get_virtual_address()
                  << "\t0x"
                  << pseg->get_file_size()
                  << "\t0x"
                  << pseg->get_memory_size()
                  << std::endl;
        // Access to segments's data
        // const char* p = reader.segments[i]->get_data()
    }

    for ( int i = 0; i < sec_num; ++i ) {
        section* psec = reader.sections[i];
        // Check section type
        if ( psec->get_type() == SHT_SYMTAB ) {
            const symbol_section_accessor symbols( reader, psec );
            for ( unsigned int j = 0; j < symbols.get_symbols_num(); ++j ) {
                std::string   name;
                Elf64_Addr    value;
                Elf_Xword     size;
                unsigned char bind;
                unsigned char type;
                Elf_Half      section_index;
                unsigned char other;

                // Read symbol properties
                symbols.get_symbol( j, name, value, size, bind,
                                    type, section_index, other );
                std::cout << j << " " << name << " " << "Address: "<< value << " Size: "<< size << std::endl;
            }
        }
    }

    return 1;
}