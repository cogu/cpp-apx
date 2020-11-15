/*****************************************************************************
* \file      client_connection.h
* \author    Conny Gustafsson
* \date      2020-11-10
* \brief     Client connection (abstract) base class
*
* Copyright (c) 2020 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
#pragma once

#include "cpp-apx/file_manager.h"
#include "cpp-apx/connection_interface.h"
#include "cpp-apx/node_manager.h"

namespace apx
{
   class ClientConnection : public ConnectionInterface
   {
   public:      
      ClientConnection() :m_file_manager{ this }, m_node_manager{ nullptr }{}
      virtual ~ClientConnection() {}

      void greeting_header_accepted();
      void connected() override;
      void disconnected() override;
      void attach_node_manager(NodeManager* node_manager);
      NodeManager* get_node_manager() const { return m_node_manager; }
      error_t build_node(char const* definition_text);
   protected:
      error_t attach_node_instance(NodeInstance* node_instance);

      FileManager m_file_manager;
      NodeManager* m_node_manager;

   };
}