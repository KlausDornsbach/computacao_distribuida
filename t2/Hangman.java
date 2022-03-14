import org.jgroups.*;
import org.jgroups.util.Util;
import java.io.*;
import java.util.Random;
import java.lang.Math;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;


public class Hangman implements Receiver {

    // basic game structures

    List<String> word_list = Arrays.asList("vector", "palindrome", "foo", "bar", "dog", "boolean");
    int word_vector_size = 6;

    List<String> body_part_list = Arrays.asList("head", "neck", "arms", "torso", "legs", "feet");
    int body_part_vector_size = 6;

    class HangmanState {
        String word;
        int number_errors;
        String masked_word;

        public HangmanState(String word) {
            this.word = word;
            number_errors = 0;
            masked_word = this.word.replaceAll(".", "*");
        }

        public void addError() {
            number_errors++;
        }

        public boolean search(char ch) {
            int word_len = this.word.length();
            boolean found = false;
            var word_char_array = this.word.toCharArray();
            var masked_word_char_array = this.masked_word.toCharArray();
            for (int i = 0; i < word_len; i++) {
                if (word_char_array[i] == ch) {
                    found = true;
                    masked_word_char_array[i] = ch;
                }
            }
            this.masked_word = String.valueOf(masked_word_char_array);
            this.word = String.valueOf(word_char_array);
            if (!found) {
                this.number_errors++;
            }
            return found;
        }
    }

    // jgroups stuff

    JChannel channel;
    protected static final String CLUSTER="hangman_game";
    String user_name = System.getProperty("user.name", "n/a");
    HangmanState state = new HangmanState("");

    public void viewAccepted(View new_view) {
        System.out.println("** view: " + new_view);
    }

    public void receive(Message msg) {
        List<String> received_state = msg.getObject();
        updateState(received_state);
        printState();
    }

    public void getState(OutputStream output) throws Exception {
        synchronized(state) {
            List<String> serializable = List.of(state.word, state.masked_word, String.valueOf(state.number_errors));
            org.jgroups.util.Util.objectToStream(serializable, new DataOutputStream(output));
        }
    }

    @SuppressWarnings("unchecked")
    public void setState(InputStream input) throws Exception {
        List<String> serialized =  (List<String>)Util.objectFromStream(new DataInputStream(input));
        updateState(serialized);
        printState();
    }

    private void start() throws Exception {
        channel = new JChannel();
        channel.setReceiver(this);
        channel.connect(CLUSTER);
        channel.getState(null, 10000);
        if (state == null || state.word.equals("")) {
            startInitialState();
        }
        printWelcome();
        eventLoop();
        channel.close();
    }

    private void eventLoop() {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        boolean quit = false;
        while (true) {
            try {
                String input;
                while (true) {
                    System.out.println("> ");
                    System.out.flush();
                    input = in.readLine().toLowerCase();
                    if (checkValidInput(input)) {
                        break;
                    }
                    if (input.startsWith("quit") || input.startsWith("exit")) {
                        quit = true;
                    }
                    break;
                }
                if (quit) {
                    break;
                }
                if (state.search(input.toCharArray()[0])){
                    printSuccessMessage();
                    if (checkWin()) {
                        printWinGameMessage();
                        break;
                    }
                } else {
                    printLooseLimbMessage();
                    if (checkLoss()) {
                        printLoseGameMessage();
                        break;
                    }
                }
                Message msg = new ObjectMessage(null, state);
                
                channel.send(msg);
            } catch (Exception e) {

            }
        }
    }

    public static void main(String[] args) throws Exception {
        new Hangman().start();
    }

    // util functions used internally for the game

    public boolean checkWin() {
        if (state.masked_word.equals(state.word)) {
            return true;
        }
        return false;
    }
    
    public boolean checkLoss() {
        if (state.number_errors >= state.word.length()) {
            return true;
        }
        return false;
    }

    public void printState() {
        System.out.println("\n\n\n--- HANGMAN ---");
        System.out.println("word: " + state.masked_word);
        System.out.println("remaining lives: " + (body_part_vector_size - state.number_errors));
        if (checkWin() || checkLoss()) {
            channel.close();
        }
    }

    public void startInitialState() {
        Random rnd = new Random();
        int i = Math.abs(rnd.nextInt()) % word_vector_size;
        HangmanState hs = new HangmanState(word_list.get(i));
        updateState(serializeHangmanState(hs));
    }

    public synchronized void updateState(List<String> state) {
        this.state.word = state.get(0);
        this.state.masked_word = state.get(1);
        this.state.number_errors = Integer.valueOf(state.get(2));
    }

    public boolean checkValidInput(String inp) {
        if (inp.length() > 1) {
            System.out.println("input must be char, too long");
            return false;
        }
        char inp_char = inp.toCharArray()[0];
        if (Character.isLetter(inp_char)) {
            return true;
        }
        System.out.println("your input is not a character!");
        return false;
    }

    public void printLooseLimbMessage() {
        System.out.println("oh no, your " + body_part_list.get(state.number_errors - 1) + " was hanged!");
    }

    public void printSuccessMessage() {
        System.out.println("you found something! the word is now " + state.masked_word);
    }

    public void printWinGameMessage() {
        System.out.println("you win!!!");
    }
    public void printLoseGameMessage() {
        System.out.println("you lose!!!");
    }

    public void printWelcome() {
        System.out.println("welcome to the Hangman game!");
        System.out.println("your word is " + state.masked_word);
        System.out.println("you have " + (word_vector_size - state.number_errors) + " lives");

    }
    public List<String> serializeHangmanState(HangmanState state) {
        return List.of(state.word, state.masked_word, String.valueOf(state.number_errors));
    }
}