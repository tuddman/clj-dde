/*
 * Copyright 2009 www.pretty-tools.com. All rights reserved.
 */

import com.pretty_tools.dde.ClipboardFormat;
import com.pretty_tools.dde.DDEException;
import com.pretty_tools.dde.client.DDEClientConversation;
import com.pretty_tools.dde.DDEMLException;

/**
 * Execute Example
 *
 * @author Alexander Kozlov
 */
public class ExecuteExample
{
    private static final String SERVICE = "MyServer";
    private static final String TOPIC = "MyTopic";

    public static void main(String[] args)
    {
        try
        {
            final DDEClientConversation conversation = new DDEClientConversation();
            // We can use UNICODE format if server prefers it
            //conversation.setTextFormat(ClipboardFormat.CF_UNICODETEXT);

            System.out.println("Connecting...");
            conversation.connect(SERVICE, TOPIC);

            System.out.println("Please, enter commands or 'exit' to quit");
            try
            {
                byte[] buff = new byte[1024];
                int ret_read = 0;
                do
                {
                    ret_read = System.in.read(buff);
                    if (ret_read > 0)
                    {
                        String data = new String(buff, 0, ret_read);
                        data = data.trim();

                        if (data.equals("exit"))
                            break;

                        System.out.println("Sending:" + data);
                        conversation.execute(data);
                    }
                }
                while ((ret_read > 0));
            }
            finally
            {
                conversation.disconnect();
            }
        }
        catch (DDEMLException e)
        {
            System.out.println("DDEMLException: 0x" + Integer.toHexString(e.getErrorCode()) + " " + e.getMessage());
        }
        catch (DDEException e)
        {
            System.out.println("DDEClientException: " + e.getMessage());
        }
        catch (Exception e)
        {
            System.out.println("Exception: " + e);
        }
    }
}
