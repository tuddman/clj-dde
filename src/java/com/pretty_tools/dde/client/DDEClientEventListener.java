/*
 * Copyright 2008-2013 www.pretty-tools.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.pretty_tools.dde.client;

/**
 * Dynamic Data Exchange asynchronous events listener.
 *
 * @author Alexander Kozlov
 */
public interface DDEClientEventListener
{
    /**
     * Fires when the application's partner in a conversation uses the DdeDisconnect function
     * to terminate the conversation.
     * <p>
     * During this event your can reconnect:
     * <pre>
     *  client.disconnect(); // should be called before connect(...)
     *  client.connect(service, topic);
     * </pre>
     * </p>
     */
    void onDisconnect();

    /**
     * Fires on item data change. See {@link DDEClientConversation#startAdvice(String)}.
     *
     * @param topic topic name
     * @param item  item name
     * @param data  data associated with the topic name and item name pair. 
     */
    void onItemChanged(String topic, String item, String data);
}
