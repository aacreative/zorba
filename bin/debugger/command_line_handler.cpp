/*
 * Copyright 2006-2008 The FLWOR Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "command_line_handler.h"

#ifndef WIN32
# include <unistd.h>
# define msleep(x) usleep(1000*x)
#else
# include <Windows.h>
# define msleep Sleep
#endif

namespace zorba { namespace debugger {
  
  using namespace ::ZORBA_TR1_NS;
  
CommandLineHandler::CommandLineHandler(
  unsigned short port,
  LockFreeConsumer<std::size_t>& aConsumer,
  LockFreeConsumer<bool>& aContinueQueue,
  EventHandler& aHandler,
  CommandPrompt& aCommandPrompt)
  : theConsumer(aConsumer),
    theContinueQueue(aContinueQueue),
    theClient(DebuggerClient::createDebuggerClient(&aHandler, port, "localhost")),
    theCommandLine(aCommandPrompt),
    theQuit(false), theContinue(false), theWaitFor(0)
  {
    addCommands();
  }
  
CommandLineHandler::~CommandLineHandler()
{
}
  
void
CommandLineHandler::execute()
{
  theClient->accept();
  std::set<std::size_t> lIdList;
  do {
    getNextId(lIdList);
    while (!theQuit && lIdList.find(theWaitFor) == lIdList.end()) {
      getNextId(lIdList);
      msleep(20);
    }
    while (!theContinueQueue.consume(theQuit)) {
      msleep(20);
    }
    theQuit = !theQuit;
    if (!theQuit) {
      theCommandLine.execute();
      while (theContinue) {
        theContinue = false;
        theCommandLine.execute();
      }
    }
  } while (!theQuit);
}
  
void
CommandLineHandler::getNextId(std::set<std::size_t>& aIdList)
{
  std::size_t result;
  if (theConsumer.consume(result)) {
    aIdList.insert(result);
  }
}

template<>
void
CommandLineHandler::handle<Status>(ZORBA_TR1_NS::tuple<> &t)
{
  theWaitFor = theClient->status();
}

template<>
void
CommandLineHandler::handle<Variables>(ZORBA_TR1_NS::tuple<> &t)
{
  theWaitFor = theClient->variables();
}

template<>
void
CommandLineHandler::handle<Quit>(ZORBA_TR1_NS::tuple<> &t)
{
  bool answered = false;
  while (!answered) {
    std::cout << "Do you really want to stop debugging and exit? (y/n) ";
    std::string lAnswer;
    std::getline(std::cin, lAnswer);
    if (lAnswer == "y" || lAnswer == "yes") {
      answered = true;
    } else if (lAnswer == "n" || lAnswer == "no") {
      theContinue = true;
      return;
    }
  }
  theWaitFor = theClient->stop();
  theClient->quit();
}
  
template<>
void
CommandLineHandler::handle<Run>(ZORBA_TR1_NS::tuple<> &t)
{
  theWaitFor = theClient->run();
}
  
template<>
void
CommandLineHandler::handle<BreakpointSet>(std::tr1::tuple<bstring, bstring, bint> &aTuple)
{
  DebuggerClient::BreakpointType lType = DebuggerClient::Line;
  bool lEnabled = true;
  if (get<0>(aTuple).first) {
    if (get<0>(aTuple).second == "disabled") {
      lEnabled = false;
    }
  }
  theWaitFor = theClient->breakpoint_set(lType,
                                          lEnabled,
                                          get<1>(aTuple).second,
                                          get<2>(aTuple).second);
}
  
template<>
void
CommandLineHandler::handle<BreakpointGet>(tuple<bint> &aTuple)
{
  theWaitFor = theClient->breakpoint_get(get<0>(aTuple).second);
}
  
template<>
void
CommandLineHandler::handle<BreakpointDel>(tuple<bint> &aTuple)
{
  theWaitFor = theClient->breakpoint_remove(get<0>(aTuple).second);
}
  
template<>
void
CommandLineHandler::handle<BreakpointList>(tuple<> &aTuple)
{
  theWaitFor = theClient->breakpoint_list();
}
  
template<>
void
CommandLineHandler::handle<StackDepth>(tuple<> &aTuple)
{
  theWaitFor = theClient->stack_depth();
}
  
template<>
void
CommandLineHandler::handle<StackGet>(tuple<bint> &aTuple)
{
  if (get<0>(aTuple).first) {
    theWaitFor = theClient->stack_get(get<0>(aTuple).second);
  } else {
    theWaitFor = theClient->stack_get();
  }
}
  
template<>
void
CommandLineHandler::handle<ContextNames>(tuple<>& aTuple)
{
  theWaitFor = theClient->context_names();
}
  
template<>
void CommandLineHandler::handle<ContextGet>(tuple<bint> &aTuple)
{
  // currently only the 2nd parameter is relevant (the context id)
  // because for the stack depth only 0 is supported (top-most)
  if (get<0>(aTuple).first) {
    theWaitFor = theClient->context_get(-1, get<0>(aTuple).second);
  } else {
    theWaitFor = theClient->context_get();
  }
}

template<>
void CommandLineHandler::handle<Source>(tuple<bint, bint, bstring> &aTuple)
{
  theWaitFor = theClient->source(
    get<2>(aTuple).second,
    get<0>(aTuple).second,
    get<1>(aTuple).second);
}
  
template<>
void CommandLineHandler::handle<Eval>(tuple<bstring>& aTuple)
{
  theWaitFor = theClient->eval(get<0>(aTuple).second);
}
  
void
CommandLineHandler::addCommands()
{
  theCommandLine << createCommand<Status>(tuple<>(), "status", *this,
                                          "Gets the status of the server");
  theCommandLine << createCommand<Variables>(tuple<>(), "variables", *this,
                                          "Gets the variables visible in the current scope");
  theCommandLine << createCommand<Quit>(tuple<>(), "quit", *this,
                                        "Stops debugging and quits the client");
  theCommandLine << createCommand<Run>(tuple<>(), "run", *this, "Run the Query");
  {
    Command<CommandLineHandler, tuple<bstring, bstring, bint>, BreakpointSet>* lCommand =
    createCommand<BreakpointSet>(tuple<bstring, bstring, bint>(), "bset", *this, "Set a breakpoint");
    lCommand->addArgument(0, "s", createArgType<tuple<bstring, bstring, bint>, std::string, 0>(tuple<bstring, bstring, bint>()),
                "breakpoint state (enabled or disabled - default: enabled)", false);
    lCommand->addArgument(1, "f", createArgType<tuple<bstring, bstring, bint>, std::string, 1>(tuple<bstring, bstring, bint>()),
                "name of the file where to stop", true);
    lCommand->addArgument(2, "l", createArgType<tuple<bstring, bstring, bint>, int, 2>(tuple<bstring, bstring, bint>()),
                "line number", true);
      
    theCommandLine << lCommand;
  }
  {
    Command<CommandLineHandler, tuple<bint>, BreakpointGet>* lCommand
    = createCommand<BreakpointGet>(tuple<bint>(), "bget", *this, 
                                  "Get information about a given breakpoint");
    lCommand->addArgument(0, "i", createArgType<tuple<bint>, int, 0>(tuple<bint>()),
                "id of the breakpoint", true);
      
    theCommandLine << lCommand;
  }
  {
    Command<CommandLineHandler, tuple<bint>, BreakpointDel>* lCommand
    = createCommand<BreakpointDel>(tuple<bint>(), "bremove", *this, "Delete a breakpoint with a given id");
    lCommand->addArgument(0, "i", createArgType<tuple<bint>, int, 0>(tuple<bint>()), "id of the breakpoint", true);
      
    theCommandLine << lCommand;
  }
  theCommandLine << createCommand<BreakpointList>(tuple<>(), "blist", *this, "List all set breakpoints");
  theCommandLine << createCommand<StackDepth>(tuple<>(), "sdepth", *this, "Get the depth of the stack");
  {
    Command<CommandLineHandler, tuple<bint>, StackGet>* lCommand
    = createCommand<StackGet>(tuple<bint>(), "sget", *this, "Get information about one or all stack frames");
    lCommand->addArgument(0, "d", createArgType<tuple<bint>, int, 0>(tuple<bint>()), "stack entry two show (show all if not provided)", false);
    theCommandLine << lCommand;
  }
  theCommandLine << createCommand<ContextNames>(tuple<>(), "cnames", *this, "Get the names of the avilable contexts");
  {
    Command<CommandLineHandler, tuple<bint>, ContextGet>* lCommand
    = createCommand<ContextGet>(tuple<bint>(), "cget", *this, "Get a context");
    lCommand->addArgument(0, "c", createArgType<tuple<bint>, int, 0>(tuple<bint>()), "id of the context", false);
      
    theCommandLine << lCommand;
  }
  {
    Command<CommandLineHandler, tuple<bint, bint, bstring>, Source>* lCommand =
      createCommand<Source>(tuple<bint, bint, bstring>(), "source", *this, "List source code");
    lCommand->addArgument(0, "b", createArgType<tuple<bint, bint, bstring>, int, 0>(tuple<bint, bint, bstring>()),
                "begin line", false);
    lCommand->addArgument(1, "e", createArgType<tuple<bint, bint, bstring>, int, 1>(tuple<bint, bint, bstring>()),
                "end line", false);
    lCommand->addArgument(2, "f", createArgType<tuple<bint, bint, bstring>, std::string, 2>(tuple<bint, bint, bstring>()),
                "file URI", false);
      
    theCommandLine << lCommand;
  }
  {
    Command<CommandLineHandler, tuple<bstring>, Eval>* lCommand
    = createCommand<Eval>(tuple<bstring>(), "eval", *this, "Evaluate a function");
    // TODO: this argument should not be here at all. Eval has the form: eval -i transaction_id -- {DATA}
    // Eval should be called with a command like: eval 1 + 3
    // - no need for an argument name
    // - everything following the fist contiguous set of whitespaces are sent as string
    lCommand->addArgument(0, "c", createArgType<tuple<bstring>, std::string, 0>(tuple<bstring>()), "command to evaluate", true);
      
    theCommandLine << lCommand;
  }
}
  
} // namespace zorba
} // namespace debugger
