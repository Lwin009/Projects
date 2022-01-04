package AutoComplete.Main;

import java.net.Socket;

import AutoComplete.data.DLB;

public class AutoComplete{

    public static void main(String[] args) {
        DLB dlb = new DLB();

        String str1 = "SHE";
        String str2 = "SELLS";
        String str3 = "SEA";
        String str4 = "SHELLS";
        String str5 = "BY";
        String str6 = "THE";
        String str7 = "SEASHORE";

        dlb.insert(str1);
        dlb.insert(str2);
        dlb.insert(str3);
        dlb.insert(str4);
        dlb.insert(str5);
        dlb.insert(str6);
        dlb.insert(str7);
        dlb.insert("words");

        System.out.println(dlb.search("DOG"));
        System.out.println(dlb.search("SELLS"));
        System.out.println(dlb.search("SHEL"));
        System.out.println(dlb.search("SHE"));
        System.out.println(dlb.search("word"));

    }
}