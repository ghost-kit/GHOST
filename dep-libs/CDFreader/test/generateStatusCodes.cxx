#include <iostream>
#include "cdf.h"
#include "QString"
#include "QStringList"

int main()
{
    std::cout << "Generating CDF status codes" << std::endl;

    for(long x = 1001; x > -2230; x--)
    {
        char text[CDF_STATUSTEXT_LEN+1];
        CDFgetStatusText(x,  text);

        QString statText = QString(text);

        if(statText.contains("Text not found")) continue;

        std::cout << "this->errorTable[" << x << "] = QString(\"" << x << " " << text << "\");" << std::endl;
    }

}
