at_translator.addDeltaRule("q0", new ArrayList() { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" },"S", new ArrayList() { "q1" }, new ArrayList() { "E" }, new ArrayList() { "" });
                                    at_translator.addDeltaRule("q0", new ArrayList() { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" },"S", new ArrayList() { "q1" }, new ArrayList() { "E" }, new ArrayList() { "" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" }, "E", new ArrayList() { "q1" }, new ArrayList() { "T","M" }, new ArrayList() { "" });
                                    at_translator.addATRule(2,"a", new ArrayList() {"b"});
                                    at_translator.addDeltaRule("q1", new ArrayList() { "+" }, "M", new ArrayList() { "q1" }, new ArrayList() { "+", "T", "M" }, new ArrayList() { "+" });
                                    at_translator.addATRule(3, "h", new ArrayList() { "s", "+", "p" });
                                    at_translator.addATRule(3, "s", new ArrayList() { "a" });
                                    at_translator.addATRule(3, "p", new ArrayList() { "b" });
                                    at_translator.addATRule(3, "a", new ArrayList() { "h" });
                                    at_translator.addATRule(3, "a", new ArrayList() { "y" });
                                    at_translator.addATRule(3, "a", new ArrayList() { "g" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "-" }, "M", new ArrayList() { "q1" }, new ArrayList() { "-", "T", "M" }, new ArrayList() { "-" });
                                    at_translator.addATRule(4, "w", new ArrayList() { "l", "-", "m" });
                                    at_translator.addATRule(4, "l", new ArrayList() { "a" });
                                    at_translator.addATRule(4, "m", new ArrayList() { "b" });
                                    at_translator.addATRule(4, "a", new ArrayList() { "w" });
                                    at_translator.addATRule(4, "a", new ArrayList() { "y" });
                                    at_translator.addATRule(4, "a", new ArrayList() { "g" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "*", "/" }, "M", new ArrayList() { "q1" }, new ArrayList() { "" }, new ArrayList() { "" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" }, "T", new ArrayList() { "q1" }, new ArrayList() { "P", "R" }, new ArrayList() { "" });
                                    at_translator.addATRule(6, "t", new ArrayList() { "x" });
                                    at_translator.addATRule(6, "b", new ArrayList() { "v" });
                                    at_translator.addATRule(6, "b", new ArrayList() { "g" });
                                    at_translator.addATRule(6, "b", new ArrayList() { "k" });
                                    at_translator.addATRule(6, "b", new ArrayList() { "y" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "*" }, "R", new ArrayList() { "q1" }, new ArrayList() { "*", "P", "Q" }, new ArrayList() { "*" });
                                    at_translator.addATRule(7, "u", new ArrayList() { "o", "*", "q" });
                                    at_translator.addATRule(7, "o", new ArrayList() { "t" });
                                    at_translator.addATRule(7, "q", new ArrayList() { "y" });
                                    at_translator.addATRule(7, "k", new ArrayList() { "y" });
                                    at_translator.addATRule(7, "g", new ArrayList() { "k" });
                                    at_translator.addATRule(7, "q", new ArrayList() { "x" });
                                    at_translator.addATRule(7, "v", new ArrayList() { "u" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "/" }, "R", new ArrayList() { "q1" }, new ArrayList() { "/", "P", "Q" }, new ArrayList() { "/" });
                                    at_translator.addATRule(8, "j", new ArrayList() { "z", "/", "c" });
                                    at_translator.addATRule(8, "z", new ArrayList() { "t" });
                                    at_translator.addATRule(8, "c", new ArrayList() { "y" });
                                    at_translator.addATRule(8, "k", new ArrayList() { "y" });
                                    at_translator.addATRule(8, "g", new ArrayList() { "k" });
                                    at_translator.addATRule(8, "c", new ArrayList() { "x" });
                                    at_translator.addATRule(8, "v", new ArrayList() { "j" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "+", "-","e" }, "R", new ArrayList() { "q1" }, new ArrayList() { "" }, new ArrayList() { "" });
                                    at_translator.addATRule(9, "y", new ArrayList() { "k" });
                                    at_translator.addATRule(9, "k", new ArrayList() { "t" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" }, "P", new ArrayList() { "q1" }, new ArrayList() { "i" }, new ArrayList() { "i" });
                                    at_translator.addATRule(10, "x", new ArrayList() { "f" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "e" }, "M", new ArrayList() { "q1" }, new ArrayList() { "" }, new ArrayList() { "" });
                                    at_translator.addATRule(11, "m", new ArrayList() { "a" });
                                    at_translator.addATRule(11, "s", new ArrayList() { "a" });
                                    at_translator.addATRule(11, "a", new ArrayList() { "h" });
                                    at_translator.addATRule(11, "a", new ArrayList() { "w" });
                                    at_translator.addDeltaRule("q1", new ArrayList() { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "*", "/", "e" }, "Q", new ArrayList() { "q1" }, new ArrayList() { "R" }, new ArrayList() { "" });
                                    at_translator.addATRule(12, "t", new ArrayList() { "v" }); //k v
