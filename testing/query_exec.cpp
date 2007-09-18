/**
 * Contains main method to execute a simple query that is passed with the first execution parameter.
 */

#include "runtime/plan_visitor.h"
#include "exprtree/normalize_visitor.h"
#include "exprtree/parsenode_print_xml_visitor.h"
#include "exprtree/parsenode_print_dot_visitor.h"
#include "parser/xquery_driver.h"
#include "errors/Error.h"
#include "runtime/zorba.h"
#include "timer.h"
#include "../store/native/basic_item_factory.h"
#include "../store/native/simple_store.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace xqp;

#ifndef _WIN32_WCE
int main(int argc, char* argv[])
#else
int _tmain(int argc, _TCHAR* argv[])
#endif
{
	Timer timer;
	timer.start();
	///application specific

	zorba::initializeZorbaEngine();
	
	SimpleStore simpleStore;
	BasicItemFactory basicItemFactory;
	static_context::init(&basicItemFactory);
	dynamic_context::init(&basicItemFactory);

	///thread specific

	zorba* zorp = zorba::allocateZorbaForNewThread();//new zorba();
	static_context* sctx_p = new static_context(NULL);//NULL);
	dynamic_context* dctx_p = new dynamic_context(NULL);//NULL);
	//add the error manager
	errors_english	*err_messages = new errors_english;///the english error messages
	ZorbaErrorAlerts	*err_manag = new ZorbaErrorAlerts(err_messages);

	library lib;
	dctx_p->set_library(&lib);

	zorp->set_static_context(sctx_p);
	zorp->set_dynamic_context(dctx_p);
	zorp->set_error_manager(err_manag);//add the error manager
	zorp->setItemFactory(&basicItemFactory);
	zorp->setStore(&simpleStore);

	xquery_driver driver(cout);
	try {

		for (++argv; argv[0]; ++argv) {
#ifndef UNICODE
			if (*argv == std::string ("-p")) {
				driver.trace_parsing = true;
			}
			else if (*argv == std::string ("-s")) {
				driver.trace_scanning = true;
			}
#else
			if (!_tcscmp(*argv, _T("-p"))) {
				driver.trace_parsing = true;
			}
			else if (!_tcscmp(*argv, _T("-s"))) {
				driver.trace_scanning = true;
			}
#endif
			else {
#ifndef UNICODE
				driver.parse(*argv);
#else
				char	testfile[1024];
				WideCharToMultiByte(CP_ACP, 0,//or CP_UTF8
														*argv, -1, 
														testfile, sizeof(testfile)/sizeof(char),
														NULL, NULL);
				driver.parse(testfile);
#endif
				parsenode* n_p = driver.get_expr();
				cout << endl;
				
				{
                    std::ofstream lXMLOutFile("query.xml");
                    ParseNodePrintXMLVisitor lPrintXMLVisitor(lXMLOutFile);
                    lPrintXMLVisitor.print(n_p);
                }    

                {
                    std::ofstream lDOTOutFile("query.dot");				
                    ParseNodePrintDOTVisitor lPrintDOTVisitor(lDOTOutFile);
                    lPrintDOTVisitor.print(n_p);
				}
				
				cout << "Syntax tree:\n";
				n_p->put(cout) << endl;
	
				normalize_visitor nvs;
				MainModule* mm_p;
				QueryBody* qb_p;
				Expr* ex_p;
	
				if ((mm_p = dynamic_cast<MainModule*>(n_p))==NULL) {
					cout << "Parse error: expecting MainModule\n";
					return -1;
				}
				if ((qb_p = dynamic_cast<QueryBody*>(&*mm_p->get_query_body()))==NULL) {
					cout << "Parse error: expecting MainModule->QueryBody\n";
					return -1;
				}
				if ((ex_p = dynamic_cast<Expr*>(&*qb_p->get_expr()))==NULL) {
					cout << "Parse error: expecting MainModule->QueryBody->Expr\n";
					return -1;
				}

				cout << "Expression tree:\n";
				ex_p->accept(nvs);
				rchandle<expr> e_h = nvs.pop_nodestack();

				cout << endl;

				if (e_h==NULL) {
					cout << "e_h==NULL\n";
					return -1;
				}
				e_h->put(cout) << endl;

				cout << "Codegen:\n";
				plan_visitor pvs;
				e_h->accept(pvs);
				Iterator_t it_h = pvs.pop_itstack();
				cout << endl;

				//cout << "iterator type = " << typeid(*it_h).name() << endl;

				cout << "Iterator tree:" << std::endl;
 				it_h->show(cout);

				cout << "\nIterator run:\n";
				if (it_h==NULL) {
					cout << "it_h==NULL\n";
					return -1;
				}
					
				while (true) {
					Item_t i_p = it_h->next();
					if (i_p == NULL)
						break;
					cout << i_p->show() << endl;
				}
				it_h->releaseResources();
			}
		}
	} catch (xqp_exception & e) {
		cout << "ZORBA EXCEPTION: " << e.get_msg() << " - " << e.what() << endl;
	} 
	// catch (exception &e) {
	//         cout << "RUNTIME EXCEPTION CATCHED! " << e.what() << std::endl;
	//     }

	zorba::destroyZorbaForCurrentThread();
	zorba::uninitializeZorbaEngine();
	
	timer.end();
	timer.print();
}
