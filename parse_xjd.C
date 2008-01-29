#include "tinyxml.h"

void parse_xjd(char *xjdName, char *compTypeInput, char **localName, int & localTasks)
{
  TiXmlDocument doc( xjdName );
  bool loadOkay = doc.LoadFile();
  
  if ( !loadOkay )
    {
      printf( "Could not load the xjd file. Error='%s'. Exiting.\n", doc.ErrorDesc() );
      exit( 1 );
    }
  
  TiXmlElement *icxjd = doc.FirstChildElement( "ICXJD" );
  TiXmlElement *components = icxjd->FirstChildElement( "components" );
  TiXmlElement *ID, *COMP_NAME, *NNODE;
  TiXmlNode *comps=0, *id, *compName, *nnode;
  const char *compType;

  // Walk all the elements in a node by value.
  for( comps = components->FirstChild( "component" );
       comps;
       comps = comps->NextSibling( "component" ) )
    {
      id = comps->FirstChild( "id" );
      ID = id->ToElement();
      compType = ID->GetText();
      if ( strcmp(compType,compTypeInput) == 0 ) { 
	compName = comps->FirstChild( "name" );
	COMP_NAME = compName->ToElement();
	*localName = const_cast<char *>(COMP_NAME->GetText());
	
	nnode = comps->FirstChild( "nNode" );
	NNODE = nnode->ToElement();
	localTasks = atoi(NNODE->GetText());
      }
    }
}
