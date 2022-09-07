using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MPTranslator
{
    // Q - множество состояний МП - автоматa
    // Sigma - алфавит входных символов
    // DeltaList - правила перехода 
    // Q0 - начальное состояние
    // F - множество конечных состояний
    //напоминалка


    //атрибут
    public class atr
    {
        public string nam { get; set; }
        public int val { get; set; }

        public atr(string name, int value)
        {
            nam = name;
            val = value;
        }

    }
    //магазинный символ
    public class at_mag
    { 
        public string name { get; set; }
        public atr at1 { get; set; }
        public atr at2 { get; set; }
        public atr at3 { get; set; }

        public at_mag(string nam, string nam1, string nam2, string nam3)
        {
            name = nam;
            at1 = new atr(nam1, -1);
            at2 = new atr(nam2, -1);
            at3 = new atr(nam3, -1);
        }

        public at_mag(string nam, string nam1, string nam2)
        {
            name = nam;
            at1 = new atr(nam1, -1);
            at2 = new atr(nam2, -1);
            at3 = new atr("", -1);
        }

        public at_mag(string namf, string nam1f)
        {
            this.name = namf;
            at1 = new atr(nam1f, -1);
            at2 = new atr("", -1);
            at3 = new atr("", -1);
        }

        public at_mag(string nam)
        {
            name = nam;
            at1 = new atr("", -1);
            at2 = new atr("", -1);
            at3 = new atr("", -1);
        }

    }

    //правило передачи атрибута по номеру правила изменения состояния
    public class at_rule
    {
        public int number { get; set; }
        public string left { get; set; }
        public ArrayList right { get; set; }

        public at_rule(int numbe, string lef, ArrayList righ)
        {
            number = numbe;
            left = lef;
            right = new ArrayList();
            right = righ;
        }
    }

    class AT_Translator : Translator
    {
        public ArrayList at_rules { get; set; } = new ArrayList();

        public ArrayList operation_symbols { get; set; } = new ArrayList();

        public bool Is_operation_symbol(string name)
        {
            foreach (at_mag at in this.operation_symbols)
            {
                if (at.name == name)
                {
                    return true;
                }
            }
            return false;
        }

        public class DeltaQSigmaGammaSix1 : DeltaQSigmaGammaSix
        {

            public ArrayList LeftTs { get; set; } = new ArrayList();
            public DeltaQSigmaGammaSix1(string LeftQ, ArrayList LeftT, string LeftZ, ArrayList RightQ, ArrayList RightZ, ArrayList RightSix) :
                base(LeftQ, LeftT[0].ToString(), LeftZ, RightQ, RightZ, RightSix)
            {
                this.LeftTs = LeftT;
            }
        } // end class DeltaQSigmaGammaSix1

        // поиск правила по состоянию. 
        public DeltaQSigmaGammaSix1 FindDelta(string Q, string a)
        {
            foreach (DeltaQSigmaGammaSix1 delta in this.DeltaList)
            {
                if (delta.LeftQ == Q && delta.LeftZ == a) return delta;
            }
            return null; // not find 
        }

        public int FindDeltaNumber(DeltaQSigmaGammaSix1 delta)
        {
            return this.DeltaList.IndexOf(delta);
        }

        public AT_Translator(ArrayList Q, ArrayList Sigma, ArrayList Gamma, string Q0, string z0, ArrayList F, ArrayList operation_symbol) :
            base(Q, Sigma, Gamma, Q0, z0, F)
        {
            this.operation_symbols = operation_symbol;
        }

        public virtual void addDeltaRule(string LeftQ, ArrayList LeftT, string LeftZ, ArrayList RightQ, ArrayList RightZ, ArrayList RightSix)
        {
            this.DeltaList.Add(new DeltaQSigmaGammaSix1(LeftQ, LeftT, LeftZ, RightQ, RightZ, RightSix));
        }

        public void addATRule(int number, string left, ArrayList right)
        {
            at_rules.Add(new at_rule(number, left, right));
        }

        public at_mag findATop(string nam) //найти для вставки в магазин  операционный символ с нужным названием и вычислить значения его атрибутов, если возможно
        {
            foreach (at_mag s in this.operation_symbols) //транслятору передавали список магазинных символов
            {
                if (s.name == nam)
                {
                    at_mag r = new at_mag(s.name, s.at1.nam, s.at2.nam, s.at3.nam);
                    return r;
                }
            }
            return null;
        }

        public at_mag findATmag(string nam) //найти для вставки в магазин символ с нужным названием и вычислить значения его атрибутов, если возможно
        {
            foreach (at_mag s in this.Gamma) //транслятору передавали список магазинных символов
            {
                if (s.name == nam)
                {
                    at_mag r = new at_mag(s.name,s.at1.nam,s.at2.nam, s.at3.nam);
                    return r;
                }
            }
            return null;
        }

        public int getATval (string name) //ищет в стеке первое вхождение атрибута и берет его значение
        {
            string Vname = null; // имя нетерминала с этим атрибутом
            foreach (at_mag at in this.Gamma)
            {
                if (at.at1.nam == name || at.at2.nam == name || at.at3.nam == name)
                {
                    Vname = at.name;
                    break;
                }
            }
            if (Vname == null)
            {
                Console.Write("Error in getATval\n");
                return -1;
            }
            foreach(at_mag at in this.Z)
            {
                if (at.name == Vname)
                {
                    if (at.at1.nam == name)
                    {
                        return at.at1.val;
                    }
                    else if (at.at2.nam == name)
                    {
                        return at.at2.val;
                    }
                    else if (at.at3.nam == name)
                    {
                        return at.at3.val;
                    }
                }
                break;
            }
            return -1;
        }

        public bool setATval(string name, int value, Stack<int> deltanumbers, int y)
        {
            int pos = 0;
            bool is_op = false;
            string Vname = null; // имя нетерминала с этим атрибутом
            foreach (at_mag at in this.Gamma)
            {
                if (at.at1.nam == name || at.at2.nam == name || at.at3.nam == name)
                {
                    Vname = at.name;
                    break;
                }
            }
            if (Vname == null)
            {
                foreach (at_mag at in this.operation_symbols)
                {
                    if (at.at1.nam == name || at.at2.nam == name || at.at3.nam == name)
                    {
                        Vname = at.name;
                        is_op = true;
                        break;
                    }
                }
                if (Vname == null)
                {
                    return false;
                }
            }
            Console.Write("Found Vname: ");
            Console.WriteLine(Vname);
            at_mag dop = new at_mag("");
            Stack d = new Stack(); //буфер
            int i = 1;
            foreach (at_mag at in this.Z)
            {
                pos++;
                if (at.name == Vname)
                {
                    Console.Write("Found in Z: ");
                    Console.Write(Vname);
                    Console.Write(" pos: ");
                    Console.WriteLine(pos);
                    if (at.at1.nam == name && at.at1.val == -1)
                    {
                        at.at1.val = value;
                        if (is_op == true) //если так присвоила значение операционному символу
                        {
                            dop = (at_mag)this.Z.Pop();
                            while (i < pos)
                            {
                                i++;
                                d.Push(dop);
                                dop = (at_mag)this.Z.Pop();
                            }

                            while (this.Z.Count > 0) {
                                dop = (at_mag)this.Z.Pop();
                                d.Push(dop);
                                while (Is_operation_symbol(dop.name) && this.Z.Count > 0) {
                                    if (dop.at3.val == -1)
                                    {
                                        break;
                                    }
                                    dop = (at_mag)this.Z.Pop();
                                    d.Push(dop);
                                }
                            }
                            this.Z.Push(at);
                            foreach (at_mag f in d) {
                                this.Z.Push(f);
                            }
                        }
                        Console.Write("ПРИСВОИЛА ЗНАЧЕНИЕ: ");
                        Console.WriteLine(value);
                        return true;
                    }
                    else if (at.at2.nam == name && at.at2.val == -1)
                    {
                        at.at2.val = value;
                        if (is_op == true) //если так присвоила значение операционному символу
                        {
                            dop = (at_mag)this.Z.Pop();
                            while (i < pos)
                            {
                                i++;
                                d.Push(dop);
                                dop = (at_mag)this.Z.Pop();
                            }
                            while (this.Z.Count > 0)
                            {
                                dop = (at_mag)this.Z.Pop();
                                d.Push(dop);
                                while (Is_operation_symbol(dop.name) && this.Z.Count > 0)
                                {
                                    if (dop.at3.val == -1)
                                    {
                                        break;
                                    }
                                    dop = (at_mag)this.Z.Pop();
                                    d.Push(dop);
                                }
                            }
                            this.Z.Push(at);
                            foreach (at_mag f in d)
                            {
                                this.Z.Push(f);
                            }
                        }
                        Console.Write("ПРИСВОИЛА ЗНАЧЕНИЕ: ");
                        Console.WriteLine(value);
                        calculate_atr3(at);
                        if (at.at3.val > -1) {
                            deltanumbers.Push(y);
                            whereAT(at.at3, deltanumbers,true);
                        }
                        return true;
                    }
                    else if (at.at3.nam == name && at.at3.val == -1)
                    {
                        at.at3.val = value;
                        if (is_op == true) //если так присвоила значение операционному символу
                        {
                            dop = (at_mag)this.Z.Pop();
                            while (i < pos)
                            {
                                i++;
                                d.Push(dop);
                                dop = (at_mag)this.Z.Pop();
                            }
                            while (this.Z.Count > 0)
                            {
                                dop = (at_mag)this.Z.Pop();
                                d.Push(dop);
                                while (Is_operation_symbol(dop.name) && this.Z.Count > 0)
                                {
                                    if (dop.at3.val == -1)
                                    {
                                        break;
                                    }
                                    dop = (at_mag)this.Z.Pop();
                                    d.Push(dop);
                                }
                            }
                            this.Z.Push(at);
                            foreach (at_mag f in d)
                            {
                                this.Z.Push(f);
                            }

                        }
                        Console.Write("ПРИСВОИЛА ЗНАЧЕНИЕ: ");
                        Console.WriteLine(value);
                        return true;
                    }
                }
            }
            return false;
        }

        public bool is_last_atr_in_rule(string nameatr, int number) //если это последний операционный символ правила
        {
            DeltaQSigmaGammaSix1 rule = (DeltaQSigmaGammaSix1)this.DeltaList[number - 1];
            if (rule.rightSix.Count > 0) {
                at_mag buf = findATop((string)(rule.rightSix[rule.rightSix.Count - 1]));
                if (buf != null) {
                    Console.Write("ИМЯ ЭТОГО МУДАКА ");
                    Console.Write(buf.name);
                    Console.Write(", buf.at3.nam: ");
                    Console.Write(buf.at3.nam);
                    Console.Write("; nameatr: ");
                    Console.WriteLine(nameatr);
                    if (buf.at3.nam == nameatr)
                    {
                        Console.WriteLine("ВЕРНУЛА ПРАВДУ ");
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

        }

        public void calculate_atr3(at_mag op) //вычислить значение третьего атрибута операционного символа
        {
            if (op.name == "+" && op.at1.val > -1 && op.at2.val > -1)
            {
                op.at3.val = op.at1.val + op.at2.val;
            }
            else if (op.name == "-" && op.at1.val > -1 && op.at2.val > -1)
            {
                op.at3.val = op.at1.val - op.at2.val;
            }
            else if (op.name == "*" && op.at1.val > -1 && op.at2.val > -1)
            {
                op.at3.val = op.at1.val * op.at2.val;
            }
            else if (op.name == "/" && op.at1.val > -1 && op.at2.val > -1)
            {
                op.at3.val = op.at1.val / op.at2.val;
            }
        }
        public void whereAT(atr at, Stack<int> deltanumbers, bool ispeek) //ищем куда вставить значение атрибута
        {
            int val;
            atr dop = new atr("",-1); // буфер
            Console.Write("In whereAT with ");
            Console.WriteLine(at.nam);
            Console.Write("deltanumbers.Peek()");
            if (deltanumbers.Count > 0)
            {
                Console.WriteLine(deltanumbers.Peek().ToString());

                foreach (var item in deltanumbers)
                {
                    Console.Write(" ");
                    Console.Write(item);
                }
                Console.WriteLine();
                ArrayList atrules1 = new ArrayList();
                ArrayList atrules3 = new ArrayList();

                foreach (at_rule r in this.at_rules) //собрать массивы всех правил которые нужно попробовать
                {
                    if (deltanumbers.Count > 0)
                    {
                        if (r.right.Contains(at.nam.ToString()) && r.number == deltanumbers.Peek())
                        {
                            Console.Write("Found rule");
                            Console.Write(" with ");
                            Console.WriteLine(at.nam);

                            if (r.right.Count == 1) //копирование значения атрибута из правой части
                            {
                                atrules1.Add(r);
                            }
                            else if (r.right.Count == 3)
                            {
                                atrules3.Add(r);
                            }
                        }
                    }
                }

                Stack<int> un = new Stack<int>();
                Stack<int> res = new Stack<int>();
                int y = deltanumbers.Pop();
                res = null;
                foreach (at_rule r in atrules1) //описать функции
                {
                    un = deltanumbers;
                    if (this.setATval(r.left, at.val, un, y) == false) //если в магазине его не лежит, но в дереве синтезирования есть
                    {
                        Console.WriteLine("HERE");
                        dop.nam = r.left;
                        dop.val = at.val;
                        foreach (at_rule p in this.at_rules)
                        {
                            if (p.right.Contains(dop.nam) && p.number == y)
                            {
                                if (un.Count > 0)
                                {
                                    if (un.Peek() != y)
                                    {
                                        un.Push(y);
                                    }
                                }
                                else
                                {
                                    un.Push(y);
                                }
                                Console.WriteLine("ВПИХНУЛИ ОБРАТНО ");
                                break;
                            }
                        }
                        whereAT(dop, un, false);
                    }
                    else
                    {
                        if (ispeek)
                        {
                            res = un;
                        }
                        break;
                    }
                }
                foreach (at_rule r in atrules3) //
                {
                    un = deltanumbers;
                    if (r.right[1].ToString() == "+")
                    {
                        val = this.getATval(r.right[0].ToString()) + this.getATval(r.right[2].ToString());
                        if (val > -2) // -1 + -1 = -2
                        {
                            if (this.setATval(r.left, at.val, un, y) == false) //если в магазине его не лежит, но в дереве синтезирования есть
                            {
                                dop.nam = r.left;
                                dop.val = val;
                                foreach (at_rule p in this.at_rules)
                                {
                                    if (p.right.Contains(dop.nam) && p.number == y)
                                    {
                                        un.Push(y);
                                        break;
                                    }
                                }
                                whereAT(dop, un, false);
                            }
                            else if (ispeek && res == null)
                            {
                                res = un;
                            }
                        }
                        else
                        {
                            Console.Write("Error in calculating\n");
                        }
                    }
                    else if (r.right[1].ToString() == "-")
                    {
                        val = this.getATval(r.right[0].ToString());
                        int val1 = this.getATval(r.right[2].ToString());
                        if (val > -1 && val1 > -1)
                        {
                            if (this.setATval(r.left, at.val, un, y) == false) //если в магазине его не лежит, но в дереве синтезирования есть
                            {
                                dop.nam = r.left;
                                dop.val = val - val1;
                                foreach (at_rule p in this.at_rules)
                                {
                                    if (p.right.Contains(dop.nam) && p.number == y)
                                    {
                                        un.Push(y);
                                        break;
                                    }
                                }
                                whereAT(dop, deltanumbers, false);
                            }
                            else if (ispeek && res == null)
                            {
                                res = un;
                            }
                        }
                        else
                        {
                            Console.Write("Error in calculating\n");
                        }
                    }
                    else if (r.right[1].ToString() == "*")
                    {
                        val = this.getATval(r.right[0].ToString());
                        int val1 = this.getATval(r.right[2].ToString());
                        if (val > -1 && val1 > -1)
                        {
                            if (this.setATval(r.left, at.val, un, y) == false) //если в магазине его не лежит, но в дереве синтезирования есть
                            {
                                dop.nam = r.left;
                                dop.val = val * val1;
                                foreach (at_rule p in this.at_rules)
                                {
                                    if (p.right.Contains(dop.nam) && p.number == y)
                                    {
                                        un.Push(y);
                                        break;
                                    }
                                }
                                whereAT(dop, un, false);
                            }
                            else if (ispeek && res == null)
                            {
                                res = un;
                            }
                        }
                        else
                        {
                            Console.Write("Error in calculating\n");
                        }
                    }
                    else if (r.right[1].ToString() == "/")
                    {
                        val = this.getATval(r.right[0].ToString());
                        int val1 = this.getATval(r.right[2].ToString());
                        if (val > -1 && val1 > -1)
                        {
                            if (this.setATval(r.left, at.val, un, y) == false) //если в магазине его не лежит, но в дереве синтезирования есть
                            {
                                dop.nam = r.left;
                                dop.val = val / val1;
                                foreach (at_rule p in this.at_rules)
                                {
                                    if (p.right.Contains(dop.nam) && p.number == y)
                                    {
                                        un.Push(y);
                                        break;
                                    }
                                }
                                whereAT(dop, un, false);

                            } //не обрабатывает деление с остатком
                            else if (ispeek && res == null)
                            {
                                res = un;
                            }
                        }
                        else
                        {
                            Console.Write("Error in calculating\n");
                        }
                    }

                }
                if (res != null)
                {
                    if (Members_in_magazine(y) && res.Peek() != y && !is_last_atr_in_rule(at.nam, y))
                    {
                        Console.Write("res != null НЕ КОНЧИЛИСЬ СИМВОЛЫ для правила ");
                        Console.WriteLine(y);
                        res.Push(y);
                    }
                    deltanumbers = res;
                }
                else
                {
                    if (Members_in_magazine(y) && deltanumbers.Peek() != y && !is_last_atr_in_rule(at.nam, y))
                    {
                        Console.Write("res == null НЕ КОНЧИЛИСЬ СИМВОЛЫ для правила ");
                        Console.WriteLine(y);
                        deltanumbers.Push(y);
                    }
                }
            }
        }

        public bool Members_in_magazine(int number) //проверяет остались ли в магазине члены правила
        {

            DeltaQSigmaGammaSix1 delta = (DeltaQSigmaGammaSix1)this.DeltaList[number - 1];
            Console.Write("this.Z.Peek()).name ");
            if (this.Z.Count > 0)
            {
                Console.WriteLine(((at_mag)this.Z.Peek()).name);
                if (delta.RightZ.Contains(((at_mag)this.Z.Peek()).name) && delta.RightZ.Count > 1)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
   

        public DeltaQSigmaGammaSix1 FindDelta(string currState, string Left, string st)
        {
            foreach (DeltaQSigmaGammaSix1 d in this.DeltaList)
            {
                if (d.LeftQ == currState && d.LeftZ == Left && d.LeftTs.Contains(st))
                {
                    return d;
                }
            }
            return null;
        }
        public override bool Execute(string str)
        {
            DeltaQSigmaGammaSix1 delta = (DeltaQSigmaGammaSix1)this.DeltaList[0];
            currState = this.Q0;
            if (this.Z.Count > 0)
            {
                this.Z.Pop();
            }
            this.Z.Push(findATmag(delta.LeftZ));
            ArrayList list = new ArrayList(); //выходная строка
            at_mag at = null; //буфер магазинного символа
            int i = 0;
            int j = 0;
            int y = 0;
            int ii = 0; //буфер для чисел
            char digit;
            Stack<int> deltanumbers = new Stack<int>(); //стек применяемых правил
            str = str + "e"; //empty step вставить "" не получается, так как это считается пустым символом, 
                             //который не отображается в строке
            string s;
            at = (at_mag)this.Z.Pop();
            for (; ; )
            {
                Console.WriteLine();
                foreach (at_mag item in this.Z)
                {
                    Console.Write(" ");
                    Console.Write(item.name);
                    Console.Write("(");
                    if (item.at1.val > -1)
                    {
                        Console.Write(item.at1.val.ToString());

                    }
                    if (item.at2.val > -1)
                    {
                        Console.Write(",");
                        Console.Write(item.at2.val.ToString());
                    }
                    if (item.at3.val > -1)
                    {
                        Console.Write(",");
                        Console.Write(item.at3.val.ToString());
                    }
                    Console.Write(")");
                }
                Console.WriteLine();
                Console.Write("str[i]: ");
                Console.WriteLine(str[i]);
                Console.Write("at.name ");
                Console.Write(at.name);
                Console.Write("(");
                if (at.at1.val > -1)
                {
                    Console.Write(at.at1.val.ToString());

                }
                if (at.at2.val > -1)
                {
                    Console.Write(",");
                    Console.Write(at.at2.val.ToString());
                }
                if (at.at3.val > -1)
                {
                    Console.Write(",");
                    Console.Write(at.at3.val.ToString());
                }
                Console.WriteLine(")");
                if (delta == null)  // в вершине стека терминал
                {
                        
                        if (at.name.ToString() == "i" && (at.at1.nam != "") && ii > 0) //если символ входной строки == верхнему символу магазина
                        {
                            Console.WriteLine("в вершине стека Операционный символ i");
                            at.at1.val = ii;
                            Console.Write("ВВЕРХ ПО i c ");
                            Console.WriteLine(at.at1.val);
                            //поиск куда нужно значение атрибутов выталкиваемого элемента
                            whereAT(at.at1, deltanumbers,true);
                            Console.WriteLine("ПОСЛЕ i");
                            list.Add(at);
                            ii = 0;
                    }
                        else if (str[i].ToString() == at.name && at.at1.nam == "")
                        {
                            Console.WriteLine("в вершине стека Терминал");
                            i++;
                            
                        }
                        else if (at.name == "i" && at.at1.nam == "")
                        {
                            digit = '0';
                            //i++;
                            ii = 0;
                            while (digit >= '0' && digit <= '9' && i <= str.Length)
                            {
                                ii = ii * 10 + digit - '0';
                                digit = str[i];
                                i++;
                            }
                            i--;
                            if (ii > 0) {
                                Console.WriteLine("в вершине стека Терминал i");
                            }
                        }
                        else if (this.Is_operation_symbol(at.name))
                        {
                            Console.WriteLine("в вершине стека Операционный символ");
                            if (deltanumbers.Count > 0)
                            {
                                y = deltanumbers.Peek();
                            }
                            foreach (var item in deltanumbers)
                            {
                                Console.Write(" ");
                                Console.Write(item);
                            }
                            Console.WriteLine();
                        
                            if (at.at1.val > -1)
                            {
                                whereAT(at.at1, deltanumbers,true);
                            if (deltanumbers.Count > 0)
                            {
                                if (deltanumbers.Peek() != y)
                                {
                                    deltanumbers.Push(y);
                                }
                            }
                            }
                            if (at.at2.val > -1)
                            {
                                whereAT(at.at2, deltanumbers,true);
                            if (deltanumbers.Count > 0)
                            {
                                if (deltanumbers.Peek() != y)
                                {
                                    deltanumbers.Push(y);
                                }
                            }
                            }
                            calculate_atr3(at);
                            if (at.at3.val > -1)
                            {
                               whereAT(at.at3, deltanumbers,true);
                            }
                        //может нужно вставить значение еще и в лист (поменявшись с тем куда вставляли местами в порядке)
                        // wherATlist(at, list);
                            list.Add(at);
                        }
                            else return false;
                        }
                 else if ((at.name == delta.LeftZ && delta.LeftTs.Contains(str[i].ToString())))  // в вершине стека Нетерминал
                 {
                    if (at.name == "Q")
                    {
                        if (deltanumbers.Count > 0)
                        {
                            if (deltanumbers.Peek() == 7 || deltanumbers.Peek() == 8)
                            {
                                deltanumbers.Pop();
                            }
                        }
                    }
                    deltanumbers.Push(FindDeltaNumber(delta) + 1);
                    
                    foreach (var item in deltanumbers)
                    {
                        Console.Write(" ");
                        Console.Write(item);
                    }
                    Console.WriteLine();
                    Console.WriteLine("в вершине стека Нетерминал");
                        s = arrToStr(delta.rightSix);
                        for (j = s.Length - 1; j >= 0; j--)
                        {
                            if (findATop(s[j].ToString()) != null) {
                                this.Z.Push(findATop(s[j].ToString()));
                            }
                        }
                        s = arrToStr(delta.RightZ);
                        /*if (s.Length == 0)
                        {
                            deltanumbers.Pop();
                        }
                        */
                    for (j = s.Length - 1; j >= 0; j--)
                        {
                            if (findATmag(s[j].ToString()) != null) {
                                this.Z.Push(findATmag(s[j].ToString()));
                            }
                        }
                        
                        //поиск куда нужно значение атрибутов выталкиваемого элемента
                        if (at.at1.val > -1)
                        {
                            whereAT(at.at1, deltanumbers,true);
                        }
                        if (at.at2.val > -1)
                        {
                            whereAT(at.at2, deltanumbers,true);
                        }
                        if (at.at3.val > -1)
                        {
                            whereAT(at.at3, deltanumbers,true);
                        }
                    }
                if (this.Z.Count != 0)
                    {
                        if (delta != null)
                        {
                            currState = arrToStr(delta.RightQ);
                        }
                        //at = (at_mag)Z.Peek();
                        at = (at_mag)this.Z.Pop();
                        delta = FindDelta(currState, at.name, str[i].ToString()); 
                    }
                    else if (str[i].ToString() == "e")
                    {
                        Console.WriteLine("СТРОКА ВЫВОДА:");
                        foreach (at_mag atu in list)
                        {
                            Console.Write("{");
                            Console.Write(atu.name);
                            Console.Write("}");
                            Console.Write("(");
                            if (atu.at1.val > -1)
                            {
                                Console.Write(atu.at1.val.ToString());
                                
                            }
                        if (atu.at2.val > -1)
                        {
                            Console.Write(",");
                            Console.Write(atu.at2.val.ToString());
                        }
                        if (atu.at3.val > -1)
                        {
                            Console.Write(",");
                            Console.Write(atu.at3.val.ToString());
                        }
                        Console.Write(")");
                    }
                        return true;
                    }
                    else return false;
                    Console.WriteLine();
            }
        }

    }
}
