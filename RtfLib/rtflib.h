#include "structures.h"



// RTF library interface
int rtf_open(char* filename, char* fonts, char* colors);				// Creates new RTF document
int rtf_close();														// Closes created RTF document
bool rtf_write_header();												// Writes RTF document header
void rtf_init();														// Sets global RTF library params
void rtf_set_fonttable(char* fonts);									// Sets new RTF document font table
void rtf_set_colortable(char* colors);									// Sets new RTF document color table
RTF_DOCUMENT_FORMAT* rtf_get_documentformat();							// Gets RTF document formatting properties
void rtf_set_documentformat(RTF_DOCUMENT_FORMAT* df);					// Sets RTF document formatting properties
bool rtf_write_documentformat();										// Writes RTF document formatting properties
RTF_SECTION_FORMAT* rtf_get_sectionformat();							// Gets RTF section formatting properties
void rtf_set_sectionformat(RTF_SECTION_FORMAT* sf);						// Sets RTF section formatting properties
bool rtf_write_sectionformat();											// Writes RTF section formatting properties
int rtf_start_section();												// Starts new RTF section
RTF_PARAGRAPH_FORMAT* rtf_get_paragraphformat();						// Gets RTF paragraph formatting properties
void rtf_set_paragraphformat(RTF_PARAGRAPH_FORMAT* pf);					// Sets RTF paragraph formatting properties
bool rtf_write_paragraphformat();										// Writes RTF paragraph formatting properties
int rtf_start_paragraph(char* text, bool newPar);						// Starts new RTF paragraph
int rtf_load_image(char* image, int width, int height);					// Loads image from file
char* rtf_bin_hex_convert(unsigned char* binary, int size);				// Converts binary data to hex
void rtf_set_defaultformat();											// Sets default RTF document formatting
int rtf_start_tablerow();												// Starts new RTF table row
int rtf_end_tablerow();													// Ends RTF table row
int rtf_start_tablecell(int rightMargin);								// Starts new RTF table cell
int rtf_end_tablecell();												// Ends RTF table cell
RTF_TABLEROW_FORMAT* rtf_get_tablerowformat();							// Gets RTF table row formatting properties
void rtf_set_tablerowformat(RTF_TABLEROW_FORMAT* rf);					// Sets RTF table row formatting properties
RTF_TABLECELL_FORMAT* rtf_get_tablecellformat();						// Gets RTF table cell formatting properties
void rtf_set_tablecellformat(RTF_TABLECELL_FORMAT* cf);					// Sets RTF table cell formatting properties
char* rtf_get_bordername(int border_type);								// Gets border name
char* rtf_get_shadingname(int shading_type, bool cell);					// Gets shading name
