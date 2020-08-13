/* 
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef _SPEECH_COMMANDS_ACTION_H_
#define _SPEECH_COMMANDS_ACTION_H_
bool speech_commands_action(int command_id);
void tcp_callback(int sock,uint16_t port,char * data,size_t len);

#endif