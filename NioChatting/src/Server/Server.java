package Server;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.Vector;

public class Server extends Thread{
    
    private static final int SERVER_PORT = 12000;
    private static final int BUF_SIZE = 256;
    private static final String EXIT = "exit";
    Selector selector;
    ServerSocketChannel ssc;
    private int clientNum = 0;

    List<ClientInfo> clientList = new Vector<ClientInfo>();
    
    public void run() {
        startServer();
    }

    public void startServer() {
        System.out.println("Server Socket is starting...");
        
        try {
            
            selector = Selector.open();
            ssc = ServerSocketChannel.open();
            ssc.configureBlocking(false);
            ssc.bind(new InetSocketAddress(SERVER_PORT));
            ssc.register(selector, SelectionKey.OP_ACCEPT);
            ByteBuffer buffer = ByteBuffer.allocate(BUF_SIZE);
            
            for(;;) {
                
                int keyCount = selector.select();
                
                if (keyCount == 0) {
                    continue;
                }
                
                Set<SelectionKey> selectedKeys = selector.selectedKeys();
                Iterator<SelectionKey> iter = selectedKeys.iterator();
                
                while(iter.hasNext()) {
                    SelectionKey selectionKey = iter.next();
                    
                    if(selectionKey.isAcceptable()) {
                        accept(selector, ssc);
                    }
                    
                    if(selectionKey.isReadable()) {
                        answerWithEcho(buffer, selectionKey);
                    }
                    
                    iter.remove();
                }
            }
            
        }catch(Exception e) {
            
            stopServer();
        }
    }

    private void answerWithEcho(ByteBuffer buffer, SelectionKey selectionKey) throws IOException {
        SocketChannel client = (SocketChannel) selectionKey.channel();
        client.read(buffer);
        
        if(new String(buffer.array()).trim().contentEquals(EXIT)) {
            client.close();
            System.out.println("Client disconnect!!");
        }
        
        buffer.flip();
        client.write(buffer);
        buffer.clear();
        
    }

    private void accept(Selector selector, ServerSocketChannel ssc) {
        try {
            
            SocketChannel sc = ssc.accept();
            sc.configureBlocking(false);
            sc.register(selector, SelectionKey.OP_READ);
            System.out.println("new client connect!");
            
            ClientInfo clientInfo = new ClientInfo(clientNum, "client", sc);
            clientNum++;
            clientList.add(clientInfo);
            
        }
        catch(Exception e) {
            
            stopServer();
            
        }

        
    }

    void stopServer() {
        try {
            
            Iterator<ClientInfo> iter = clientList.iterator();
            
            while(iter.hasNext()) {
                //Client client = iter.next();
                //client.socketChannel.close();
                iter.remove();
            }
            
            if(ssc != null && ssc.isOpen()) {
                ssc.close();
            }
            
            if(selector != null && selector.isOpen()) {
                selector.close();
            }
            
        }catch (Exception e) {
            System.out.println("Server has Problem when shutting down server...");
        }
    }
}
