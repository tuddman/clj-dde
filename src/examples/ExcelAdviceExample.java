/*
 * Copyright 2009 www.pretty-tools.com. All rights reserved.
 */

import com.pretty_tools.dde.DDEException;
import com.pretty_tools.dde.client.DDEClientConversation;
import com.pretty_tools.dde.client.DDEClientEventListener;
import com.pretty_tools.dde.DDEMLException;

/**
 * Excel Advice Example.
 *
 * @author Alexander Kozlov
 */
public class ExcelAdviceExample
{
    public static void main(String[] args)
    {
        try
        {
            // DDE client
            final DDEClientConversation conversation = new DDEClientConversation();
            // We can use UNICODE format if server prefers it
            //conversation.setTextFormat(ClipboardFormat.CF_UNICODETEXT);

            conversation.setEventListener(new DDEClientEventListener()
            {
                public void onDisconnect()
                {
                    System.out.println("onDisconnect()");
                }

                public void onItemChanged(String topic, String item, String data)
                {
                    System.out.println("onItemChanged(" + topic + "," + item + "," + data.trim() + ")");
                }
            });

            System.out.println("Connecting...");
            conversation.connect("Excel", "Sheet1");
            try
            {
                conversation.startAdvice("R1C1");

                System.out.println("Press Enter to quit");
                System.in.read();

                conversation.stopAdvice("R1C1");
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