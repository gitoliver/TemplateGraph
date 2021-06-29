#ifndef LAZY_PRINTERS_HPP
#define LAZY_PRINTERS_HPP

#include <iostream>

inline std::string starLine = "****************************************\n";

// LAZY INFORMATION OUTPUT
inline void lazyInfo(int t_lineCalled, const char *t_funcName)
{
  std::cout << starLine << "\tINFORMATION: " << t_funcName << "\n" << starLine;
  std::cout << "Function Name: " << t_funcName << std::endl;
  std::cout << "Line Number: " << t_lineCalled << std::endl << std::endl;
}

inline void lazyInfo(int t_lineCalled, const char *t_funcName, std::string t_infoToPass)
{
  std::cout << starLine << "\t\tINFORMATION\n" << starLine;
  std::cout << "Function Name: " << t_funcName << std::endl;
  std::cout << "Line Number: " << t_lineCalled << std::endl;
  std::cout << "Msg: " << t_infoToPass << std::endl << std::endl;
}

/* NOTE: Please note that these are to temporarily keep track of unwanted behavior
 * 			and we will eventually replace these with correct error handling etc.
 * 			but while the program and logic are in flux I would prefer to keep
 * 			our error handling ambiguous.
 *
 */
inline void badBehavior(int t_lineBroke, const char *t_funcNameBroke)
{
  std::cout << starLine << "\t\tBORKED\n" << starLine;
  std::cout << "Borked Function Name: " << t_funcNameBroke << std::endl;
  std::cout << "Line Number: " << t_lineBroke << std::endl << std::endl;
}

inline void badBehavior(int t_lineBroke, const char *t_funcNameBroke, std::string t_infoToPass)
{
  std::cout << starLine << "\t\tBORKED\n" << starLine;
  std::cout << "Borked Function Name: " << t_funcNameBroke << std::endl;
  std::cout << "Line Number: " << t_lineBroke << std::endl << std::endl;
  std::cout << "Msg: " << t_infoToPass << std::endl << std::endl;
}

#endif // LAZY_PRINTERS_HPP
