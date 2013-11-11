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

package com.pretty_tools.dde;

/**
 * DDE Exception.
 *
 * @author Alexander Kozlov
 */
public class DDEException extends Exception
{
    public DDEException()
    {
    }

    public DDEException(String message)
    {
        super(message);
    }

    public DDEException(String message, Throwable cause)
    {
        super(message, cause);
    }

    public DDEException(Throwable cause)
    {
        super(cause);
    }
}
