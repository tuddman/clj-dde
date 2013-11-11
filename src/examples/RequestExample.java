/*
 * Copyright 2009 www.pretty-tools.com. All rights reserved.
 */

import com.pretty_tools.dde.DDEException;
import com.pretty_tools.dde.client.DDEClientConversation;
import com.pretty_tools.dde.DDEMLException;

/**
 * Request Example with several threads.
 *
 * @author Alexander Kozlov
 */
public class RequestExample implements Runnable
{
    private static final String SERVICE = "MyServer";
    private static final String TOPIC = "MyTopic";

    public RequestExample()
    {
    }

    public void run()
    {
        try
        {
            final DDEClientConversation conversation = new DDEClientConversation();
            // We can use UNICODE format if server prefers it
            //conversation.setTextFormat(ClipboardFormat.CF_UNICODETEXT);

            System.out.println("Connecting...");
            conversation.connect(SERVICE, TOPIC);

            for (int i = 0; i < 1000; ++i)
            {
                try
                {
                    System.out.println("Requesting data... i=" + i);
                    String data = conversation.request("MyRequest");
                    System.out.println("Item(" + i + ") data: " + data);
                }
                catch (DDEException e)
                {
                    System.out.println("Exception on request: " + e);
                }
            }

            System.out.println("Disconnecting...");
            conversation.disconnect();
            System.out.println("Exit from thread");
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

    public static void main(String[] args)
    {
        try
        {
            Thread thread = new Thread(new RequestExample());
            Thread thread2 = new Thread(new RequestExample());

            thread.start();
            thread2.start();

            thread.join();
            thread2.join();
        }
        catch (Exception e)
        {
            System.out.println("Exception: " + e);
        }
    }
}
