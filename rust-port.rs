// https://norvig.com/sudoku.html
// https://github.com/norvig/pytudes/blob/master/py/sudoku.py

/*

00 01 02 | 03 04 05 | 06 07 08
09 10 11 | 12 13 14 | 15 16 17
18 19 20 | 21 22 23 | 24 25 26
------------------------------
27 28 29 | 30 31 32 | 33 34 35
36 37 38 | 39 40 41 | 42 43 44
45 46 47 | 48 49 50 | 51 52 53
------------------------------
54 55 56 | 57 58 59 | 60 61 62
63 64 65 | 66 67 68 | 69 70 71
72 73 74 | 75 76 77 | 78 79 80

 */

static INITIAL_VALUES: u16 = 0b0001_1111_1111;

static GRID: [u16; 81] = [INITIAL_VALUES; 81];

const fn horizontal_peers(i: u8) -> u128 {
    0b0001_1111_1111u128 << i / 9 * 9
}

const fn vertical_peers(i: u8) -> u128 {
    let i = i % 9;

    1u128 << i | 1 << i + 9 | 1 << i + 18 |
    1 << i + 27 | 1 << i + 36 | 1 << i + 45 |
    1 << i + 54 | 1 << i + 63 | 1 << i + 72
}

const fn unit_top_left(i: u8) -> u8 {
    (i / 27) * 27 + (i % 9 / 3) * 3
}

const fn unit_peers(i: u8) -> u128 {
    0b0001_1100_0000_1110_0000_0111u128 << unit_top_left(i)
}

const fn peers(i: u8) -> u128 {
    (horizontal_peers(i) | vertical_peers(i) | unit_peers(i)) & !(1u128 << i)
}

static PEERS: [u128; 81] = [
    peers(00), peers(01), peers(02), peers(03), peers(04), peers(05), peers(06), peers(07), peers(08),
    peers(09), peers(10), peers(11), peers(12), peers(13), peers(14), peers(15), peers(16), peers(17),
    peers(18), peers(19), peers(20), peers(21), peers(22), peers(23), peers(24), peers(25), peers(26),
    peers(27), peers(28), peers(29), peers(30), peers(31), peers(32), peers(33), peers(34), peers(35),
    peers(36), peers(37), peers(38), peers(39), peers(40), peers(41), peers(42), peers(43), peers(44),
    peers(45), peers(46), peers(47), peers(48), peers(49), peers(50), peers(51), peers(52), peers(53),
    peers(54), peers(55), peers(56), peers(57), peers(58), peers(59), peers(60), peers(61), peers(62),
    peers(63), peers(64), peers(65), peers(66), peers(67), peers(68), peers(69), peers(70), peers(71),
    peers(72), peers(73), peers(74), peers(75), peers(76), peers(77), peers(78), peers(79), peers(80),
];

macro_rules! is_not_a_peer {
    ($peers:expr, $square:expr) => ($peers & 1u128 << $square == 0);
}

macro_rules! value_was_already_eliminated {
    ($grid:expr, $square:expr, $value:expr) => ($grid[$square] & $value == 0);
}

macro_rules! eliminate_value {
    ($grid:expr, $square:expr, $value:expr) => ($grid[$square] &= !$value); 
}

macro_rules! no_valid_values {
    ($grid:expr, $square:expr) => ($grid[$square] == 0);
}

macro_rules! square_is_solved {
    ($grid:expr, $square:expr) => ($grid[$square] & ($grid[$square] - 1) == 0);
}

macro_rules! cannot_eliminate_value_from_peers {
    ($grid:expr, $square:expr) => (!eliminate_value_from_peers($grid, $square, $grid[$square]));
}

fn eliminate_value_from_peers(grid: &mut [u16; 81], square: usize, value: u16) -> bool {
    let peers = PEERS[square];

    for square in 0..81 {
        if is_not_a_peer!(peers, square) {
            continue
        }
        
        if value_was_already_eliminated!(grid, square, value) {
            continue
        }

        eliminate_value!(grid, square, value);

        if no_valid_values!(grid, square) {
            return false
        }

        if square_is_solved!(grid, square) && cannot_eliminate_value_from_peers!(grid, square) {
            return false
        }
    }
    
    true
}

fn assign(grid: &mut [u16; 81], square: usize, value: u16) -> bool {
    grid[square] = value;

    eliminate_value_from_peers(grid, square, value)
}

fn unsolved_square_with_fewest_values(grid: &[u16; 81]) -> Option<usize> {
    let none_found = 10u16;
    let mut min_remaining = none_found;
    let mut min_square = 0;

    for square in 0..81 {
        let values_remaining = grid[square].count_ones() as u16;

        if values_remaining > 1 && values_remaining < min_remaining {
            min_remaining = values_remaining;
            min_square = square;

            if values_remaining == 2 {
                break
            }
        }
    }

    if min_remaining == none_found {
        return None
    }

    Some(min_square)
}

fn search(grid: &[u16; 81]) -> bool {
    match unsolved_square_with_fewest_values(grid) {
        Some(square) => {
            let values = grid[square];
            
            for b in 0..9 {
                let value = 1u16 << b;
                
                if values & value == 0 {
                    continue
                }
                
                let mut grid = grid.clone();
                
                if assign(&mut grid, square, value) && search(&grid) {
                    return true
                }
            }
        },
        None => {
            //print_grid(grid);
            return true
        }
    }
    
    false
}

fn print_grid(grid: &[u16; 81]) {
    for i in 0..81 {
        let values = grid[i];

        for b in 0..9 {
            if values & 1u16 << b > 0 {
                print!("{}", b + 1);
            }
        }

        print!(" ");
        
        if (i + 1) % 9 == 0 {
            println!("");
        }
    }
}

fn solve(puzzle: &str, mut grid: &mut [u16; 81]) -> bool {
    for (square, value) in puzzle.chars().enumerate() {
        match value {
            '1'...'9' => {
                let value = 1u16 << (value as u8) - 49;
                
                if !assign(&mut grid, square, value) {
                    println!("Could not assign");
                    return false
                }
            },
            '.' | '0' => continue,
            _ => assert!(false, "Invalid input: {}", value)
        }
    }
    
    search(&mut grid)
}

fn main() {
    //let puzzle = "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......";
    //let puzzle = "003020600900305001001806400008102900700000008006708200002609500800203009005010300";
    //let puzzle = ".....6....59.....82....8....45........3........6..3.54...325..6..................";
    //let puzzle = "85...24..72......9..4.........1.7..23.5...9...4...........8..7..17..........36.4.";
    //let puzzle = "..53.....8......2..7..1.5..4....53...1..7...6..32...8..6.5....9..4....3......97..";
    //let puzzle = "4.....5.8.3..........7......2.....6.....5.8......1.......6.3.7.5..2.....1.8......";

    /*
    let mut grid = GRID.clone();

    println!("{}", puzzle);
    println!("");

    if solve(&puzzle, &mut grid) {
        println!("Solved!");
    }
     */

    profile_example_puzzles()
}

macro_rules! current_millis {
    () => (SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis())
}

fn timed_solve(puzzle: &str, mut grid: &mut [u16; 81]) -> Option<u128> {
    use std::time::{SystemTime, UNIX_EPOCH};

    let start_millis = current_millis!();

    if !solve(&puzzle, &mut grid) {
        return None
    }

    let end_millis = current_millis!();

    Some(end_millis - start_millis)
}

fn profile_puzzles(puzzles: &[&str]) -> Option<(u128, u128, u128)> {
    let mut total_time = 0u128;
    let mut max_time = 0u128;
    let mut count = 0;
    
    for puzzle in puzzles.iter() {
        let mut grid = GRID.clone();
        
        match timed_solve(&puzzle, &mut grid) {
            Some(puzzle_time) => {
                total_time += puzzle_time;

                if puzzle_time > max_time {
                    max_time = puzzle_time;
                }
            }
            None => return None
        }

        count += 1;
    }

    Some((count, total_time / count, max_time))
}

fn profile_easy_50_puzzles() {
    match profile_puzzles(&EASY_50_PUZZLES) {
        Some((count, avg_time, max_time)) => {
            println!("Solved {} easy puzzles (avg {} millis, max {} millis).",
                     count, avg_time, max_time)
        },
        None => println!("Failed to solve easy puzzles")
    }
}

fn profile_top_95_puzzles() {
    match profile_puzzles(&TOP_95_PUZZLES) {
        Some((count, avg_time, max_time)) => {
            println!("Solved {} hard puzzles (avg {} millis, max {} millis).",
                     count, avg_time, max_time)
        },
        None => println!("Failed to solve hard puzzles")
    }
}

fn profile_hardest_11_puzzles() {
    match profile_puzzles(&HARDEST_11_PUZZLES) {
        Some((count, avg_time, max_time)) => {
            println!("Solved {} hardest puzzles (avg {} millis, max {} millis).",
                     count, avg_time, max_time)
        },
        None => println!("Failed to solve hardest puzzles")
    }
}

fn profile_example_puzzles() {
    profile_easy_50_puzzles();
    profile_top_95_puzzles();
    profile_hardest_11_puzzles();
}

#[cfg(test)]
mod sudoku_setup_tests {
    use super::*;
    
    #[test]
    fn there_are_81_squares() {
        assert_eq!(GRID.iter().count(), 81);
    }

    #[test]
    fn test_square_top_left() {
        for i in 0..81 {
            match i {
                0...2 | 9...11 | 18...20 => assert_eq!(unit_top_left(i), 0),
                3...5 | 12...14 | 21...23 => assert_eq!(unit_top_left(i), 3),
                6...8 | 15...17 | 24...26 => assert_eq!(unit_top_left(i), 6),
                27...29 | 36...38 | 45...47 => assert_eq!(unit_top_left(i), 27),
                30...32 | 39...41 | 48...50 => assert_eq!(unit_top_left(i), 30),
                33...35 | 42...44 | 51...53 => assert_eq!(unit_top_left(i), 33),
                54...56 | 63...65 | 72...74 => assert_eq!(unit_top_left(i), 54),
                57...59 | 66...68 | 75...77 => assert_eq!(unit_top_left(i), 57),
                60...62 | 69...71 | 78...80 => assert_eq!(unit_top_left(i), 60),
                _ => assert_eq!(false, true),
            }
        }
    }

    #[test]
    fn each_square_has_8_horizontal_peers() {
        for i in 0..81 {
            // at this point the square is not eliminated from its valid peers
            assert_eq!(horizontal_peers(i).count_ones() - 1, 8);
        }
    }

    #[test]
    fn each_square_has_8_vertical_peers() {
        for i in 0..81 {
            // at this point the square is not eliminated from its valid peers
            assert_eq!(vertical_peers(i).count_ones() - 1, 8);
        }
    }

    #[test]
    fn each_square_has_8_unit_peers() {
        for i in 0..81 {
            // at this point the square is not eliminated from its valid peers
            assert_eq!(unit_peers(i).count_ones() - 1, 8);
        }
    }

    #[test]
    fn test_each_square_has_20_peers() {
        for i in 0..81 {
            assert_eq!(PEERS[i].count_ones(), 20);
        }
    }
}

#[cfg(test)]
mod sudoku_logic_tests {
    use super::*;

    #[test]
    fn eliminate_removes_known_value_from_all_peers() {
        let mut grid = GRID.clone();
        let known_value = 0b1000u16;
        let square = 0;

        grid[square] = known_value;

        assert_eq!(eliminate_value_from_peers(&mut grid, square, known_value), true);

        for square in 0..81 {
            let expected = match square {
                0 => known_value,
                1...11 | 18...20 | 27 | 36 | 45 | 54 | 63 | 72 => 0b0001_1111_0111,
                _ => INITIAL_VALUES,
            };
            
            assert_eq!(grid[square], expected);
        }
    }

    #[test]
    fn eliminate_does_nothing_to_peers_without_values_to_remove() {
        let mut grid = GRID.clone();
        let known_value = 0b1u16;
        let peer_value = 0b10u16;
        let square = 0;
        let peer = 4;

        grid[square] = known_value;
        grid[peer] = peer_value;

        assert_eq!(eliminate_value_from_peers(&mut grid, square, known_value), true);

        assert_eq!(grid[square], known_value);
        assert_eq!(grid[peer], peer_value);
    }    
}

#[cfg(test)]
mod sudoku_integration_tests {
    use super::*;

    fn puzzle_can_be_solved(puzzle: &str) -> bool {
        let mut grid = GRID.clone();

        solve(puzzle, &mut grid)
    }

    #[test]
    fn invalid_puzzles_do_not_solve() {
        let puzzles = [
            "111111111111111111111111111111111111111111111111111111111111111111111111111111111",
            "11...............................................................................",
            "11.............................................................................22",
            "11...................................55555.....................................22",
            "11..................44444444444444...55555.33333333..9999999999999999999999999.22",
        ];

        for puzzle in puzzles.iter() {
            assert_eq!(puzzle_can_be_solved(&puzzle), false);
        }
    }
    
    #[test]
    fn solves_easy_50_samples() {
        assert_eq!(puzzle_can_be_solved(EASY_50_PUZZLES[0]), true);
        assert_eq!(puzzle_can_be_solved(EASY_50_PUZZLES[16]), true);
        assert_eq!(puzzle_can_be_solved(EASY_50_PUZZLES[22]), true);
        assert_eq!(puzzle_can_be_solved(EASY_50_PUZZLES[37]), true);
        assert_eq!(puzzle_can_be_solved(EASY_50_PUZZLES[45]), true);
    }

    #[test]
    fn solves_hardest_11_samples() {
        assert_eq!(puzzle_can_be_solved(&HARDEST_11_PUZZLES[1]), true);
        assert_eq!(puzzle_can_be_solved(&HARDEST_11_PUZZLES[2]), true);
        assert_eq!(puzzle_can_be_solved(&HARDEST_11_PUZZLES[5]), true);
        assert_eq!(puzzle_can_be_solved(&HARDEST_11_PUZZLES[9]), true);
        assert_eq!(puzzle_can_be_solved(&HARDEST_11_PUZZLES[10]), true);
    }

    #[test]
    fn solves_top_95_samples() {
        assert_eq!(puzzle_can_be_solved(&TOP_95_PUZZLES[2]), true);
        assert_eq!(puzzle_can_be_solved(&TOP_95_PUZZLES[24]), true);
        assert_eq!(puzzle_can_be_solved(&TOP_95_PUZZLES[67]), true);
        assert_eq!(puzzle_can_be_solved(&TOP_95_PUZZLES[73]), true);
        assert_eq!(puzzle_can_be_solved(&TOP_95_PUZZLES[81]), true);
    }
}


static EASY_50_PUZZLES: [&str; 50] = [
    "003020600900305001001806400008102900700000008006708200002609500800203009005010300",
    "200080300060070084030500209000105408000000000402706000301007040720040060004010003",
    "000000907000420180000705026100904000050000040000507009920108000034059000507000000",
    "030050040008010500460000012070502080000603000040109030250000098001020600080060020",
    "020810740700003100090002805009040087400208003160030200302700060005600008076051090",
    "100920000524010000000000070050008102000000000402700090060000000000030945000071006",
    "043080250600000000000001094900004070000608000010200003820500000000000005034090710",
    "480006902002008001900370060840010200003704100001060049020085007700900600609200018",
    "000900002050123400030000160908000000070000090000000205091000050007439020400007000",
    "001900003900700160030005007050000009004302600200000070600100030042007006500006800",
    "000125400008400000420800000030000095060902010510000060000003049000007200001298000",
    "062340750100005600570000040000094800400000006005830000030000091006400007059083260",
    "300000000005009000200504000020000700160000058704310600000890100000067080000005437",
    "630000000000500008005674000000020000003401020000000345000007004080300902947100080",
    "000020040008035000000070602031046970200000000000501203049000730000000010800004000",
    "361025900080960010400000057008000471000603000259000800740000005020018060005470329",
    "050807020600010090702540006070020301504000908103080070900076205060090003080103040",
    "080005000000003457000070809060400903007010500408007020901020000842300000000100080",
    "003502900000040000106000305900251008070408030800763001308000104000020000005104800",
    "000000000009805100051907420290401065000000000140508093026709580005103600000000000",
    "020030090000907000900208005004806500607000208003102900800605007000309000030020050",
    "005000006070009020000500107804150000000803000000092805907006000030400010200000600",
    "040000050001943600009000300600050002103000506800020007005000200002436700030000040",
    "004000000000030002390700080400009001209801307600200008010008053900040000000000800",
    "360020089000361000000000000803000602400603007607000108000000000000418000970030014",
    "500400060009000800640020000000001008208000501700500000000090084003000600060003002",
    "007256400400000005010030060000508000008060200000107000030070090200000004006312700",
    "000000000079050180800000007007306800450708096003502700700000005016030420000000000",
    "030000080009000500007509200700105008020090030900402001004207100002000800070000090",
    "200170603050000100000006079000040700000801000009050000310400000005000060906037002",
    "000000080800701040040020030374000900000030000005000321010060050050802006080000000",
    "000000085000210009960080100500800016000000000890006007009070052300054000480000000",
    "608070502050608070002000300500090006040302050800050003005000200010704090409060701",
    "050010040107000602000905000208030501040070020901080406000401000304000709020060010",
    "053000790009753400100000002090080010000907000080030070500000003007641200061000940",
    "006080300049070250000405000600317004007000800100826009000702000075040190003090600",
    "005080700700204005320000084060105040008000500070803010450000091600508007003010600",
    "000900800128006400070800060800430007500000009600079008090004010003600284001007000",
    "000080000270000054095000810009806400020403060006905100017000620460000038000090000",
    "000602000400050001085010620038206710000000000019407350026040530900020007000809000",
    "000900002050123400030000160908000000070000090000000205091000050007439020400007000",
    "380000000000400785009020300060090000800302009000040070001070500495006000000000092",
    "000158000002060800030000040027030510000000000046080790050000080004070100000325000",
    "010500200900001000002008030500030007008000500600080004040100700000700006003004050",
    "080000040000469000400000007005904600070608030008502100900000005000781000060000010",
    "904200007010000000000706500000800090020904060040002000001607000000000030300005702",
    "000700800006000031040002000024070000010030080000060290000800070860000500002006000",
    "001007090590080001030000080000005800050060020004100000080000030100020079020700400",
    "000003017015009008060000000100007000009000200000500004000000020500600340340200000",
    "300200000000107000706030500070009080900020004010800050009040301000702000000008006"
];

static HARDEST_11_PUZZLES: [&str; 11] = [
    "85...24..72......9..4.........1.7..23.5...9...4...........8..7..17..........36.4.",
    "..53.....8......2..7..1.5..4....53...1..7...6..32...8..6.5....9..4....3......97..",
    "12..4......5.69.1...9...5.........7.7...52.9..3......2.9.6...5.4..9..8.1..3...9.4",
    "...57..3.1......2.7...234......8...4..7..4...49....6.5.42...3.....7..9....18.....",
    "7..1523........92....3.....1....47.8.......6............9...5.6.4.9.7...8....6.1.",
    "1....7.9..3..2...8..96..5....53..9...1..8...26....4...3......1..4......7..7...3..",
    "1...34.8....8..5....4.6..21.18......3..1.2..6......81.52..7.9....6..9....9.64...2",
    "...92......68.3...19..7...623..4.1....1...7....8.3..297...8..91...5.72......64...",
    ".6.5.4.3.1...9...8.........9...5...6.4.6.2.7.7...4...5.........4...8...1.5.2.3.4.",
    "7.....4...2..7..8...3..8.799..5..3...6..2..9...1.97..6...3..9...3..4..6...9..1.35",
    "....7..2.8.......6.1.2.5...9.54....8.........3....85.1...3.2.8.4.......9.7..6...."
];

static TOP_95_PUZZLES: [&str; 95] = [
    "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......",
    "52...6.........7.13...........4..8..6......5...........418.........3..2...87.....",
    "6.....8.3.4.7.................5.4.7.3..2.....1.6.......2.....5.....8.6......1....",
    "48.3............71.2.......7.5....6....2..8.............1.76...3.....4......5....",
    "....14....3....2...7..........9...3.6.1.............8.2.....1.4....5.6.....7.8...",
    "......52..8.4......3...9...5.1...6..2..7........3.....6...1..........7.4.......3.",
    "6.2.5.........3.4..........43...8....1....2........7..5..27...........81...6.....",
    ".524.........7.1..............8.2...3.....6...9.5.....1.6.3...........897........",
    "6.2.5.........4.3..........43...8....1....2........7..5..27...........81...6.....",
    ".923.........8.1...........1.7.4...........658.........6.5.2...4.....7.....9.....",
    "6..3.2....5.....1..........7.26............543.........8.15........4.2........7..",
    ".6.5.1.9.1...9..539....7....4.8...7.......5.8.817.5.3.....5.2............76..8...",
    "..5...987.4..5...1..7......2...48....9.1.....6..2.....3..6..2.......9.7.......5..",
    "3.6.7...........518.........1.4.5...7.....6.....2......2.....4.....8.3.....5.....",
    "1.....3.8.7.4..............2.3.1...........958.........5.6...7.....8.2...4.......",
    "6..3.2....4.....1..........7.26............543.........8.15........4.2........7..",
    "....3..9....2....1.5.9..............1.2.8.4.6.8.5...2..75......4.1..6..3.....4.6.",
    "45.....3....8.1....9...........5..9.2..7.....8.........1..4..........7.2...6..8..",
    ".237....68...6.59.9.....7......4.97.3.7.96..2.........5..47.........2....8.......",
    "..84...3....3.....9....157479...8........7..514.....2...9.6...2.5....4......9..56",
    ".98.1....2......6.............3.2.5..84.........6.........4.8.93..5...........1..",
    "..247..58..............1.4.....2...9528.9.4....9...1.........3.3....75..685..2...",
    "4.....8.5.3..........7......2.....6.....5.4......1.......6.3.7.5..2.....1.9......",
    ".2.3......63.....58.......15....9.3....7........1....8.879..26......6.7...6..7..4",
    "1.....7.9.4...72..8.........7..1..6.3.......5.6..4..2.........8..53...7.7.2....46",
    "4.....3.....8.2......7........1...8734.......6........5...6........1.4...82......",
    ".......71.2.8........4.3...7...6..5....2..3..9........6...7.....8....4......5....",
    "6..3.2....4.....8..........7.26............543.........8.15........8.2........7..",
    ".47.8...1............6..7..6....357......5....1..6....28..4.....9.1...4.....2.69.",
    "......8.17..2........5.6......7...5..1....3...8.......5......2..4..8....6...3....",
    "38.6.......9.......2..3.51......5....3..1..6....4......17.5..8.......9.......7.32",
    "...5...........5.697.....2...48.2...25.1...3..8..3.........4.7..13.5..9..2...31..",
    ".2.......3.5.62..9.68...3...5..........64.8.2..47..9....3.....1.....6...17.43....",
    ".8..4....3......1........2...5...4.69..1..8..2...........3.9....6....5.....2.....",
    "..8.9.1...6.5...2......6....3.1.7.5.........9..4...3...5....2...7...3.8.2..7....4",
    "4.....5.8.3..........7......2.....6.....5.8......1.......6.3.7.5..2.....1.8......",
    "1.....3.8.6.4..............2.3.1...........958.........5.6...7.....8.2...4.......",
    "1....6.8..64..........4...7....9.6...7.4..5..5...7.1...5....32.3....8...4........",
    "249.6...3.3....2..8.......5.....6......2......1..4.82..9.5..7....4.....1.7...3...",
    "...8....9.873...4.6..7.......85..97...........43..75.......3....3...145.4....2..1",
    "...5.1....9....8...6.......4.1..........7..9........3.8.....1.5...2..4.....36....",
    "......8.16..2........7.5......6...2..1....3...8.......2......7..3..8....5...4....",
    ".476...5.8.3.....2.....9......8.5..6...1.....6.24......78...51...6....4..9...4..7",
    ".....7.95.....1...86..2.....2..73..85......6...3..49..3.5...41724................",
    ".4.5.....8...9..3..76.2.....146..........9..7.....36....1..4.5..6......3..71..2..",
    ".834.........7..5...........4.1.8..........27...3.....2.6.5....5.....8........1..",
    "..9.....3.....9...7.....5.6..65..4.....3......28......3..75.6..6...........12.3.8",
    ".26.39......6....19.....7.......4..9.5....2....85.....3..2..9..4....762.........4",
    "2.3.8....8..7...........1...6.5.7...4......3....1............82.5....6...1.......",
    "6..3.2....1.....5..........7.26............843.........8.15........8.2........7..",
    "1.....9...64..1.7..7..4.......3.....3.89..5....7....2.....6.7.9.....4.1....129.3.",
    ".........9......84.623...5....6...453...1...6...9...7....1.....4.5..2....3.8....9",
    ".2....5938..5..46.94..6...8..2.3.....6..8.73.7..2.........4.38..7....6..........5",
    "9.4..5...25.6..1..31......8.7...9...4..26......147....7.......2...3..8.6.4.....9.",
    "...52.....9...3..4......7...1.....4..8..453..6...1...87.2........8....32.4..8..1.",
    "53..2.9...24.3..5...9..........1.827...7.........981.............64....91.2.5.43.",
    "1....786...7..8.1.8..2....9........24...1......9..5...6.8..........5.9.......93.4",
    "....5...11......7..6.....8......4.....9.1.3.....596.2..8..62..7..7......3.5.7.2..",
    ".47.2....8....1....3....9.2.....5...6..81..5.....4.....7....3.4...9...1.4..27.8..",
    "......94.....9...53....5.7..8.4..1..463...........7.8.8..7.....7......28.5.26....",
    ".2......6....41.....78....1......7....37.....6..412....1..74..5..8.5..7......39..",
    "1.....3.8.6.4..............2.3.1...........758.........7.5...6.....8.2...4.......",
    "2....1.9..1..3.7..9..8...2.......85..6.4.........7...3.2.3...6....5.....1.9...2.5",
    "..7..8.....6.2.3...3......9.1..5..6.....1.....7.9....2........4.83..4...26....51.",
    "...36....85.......9.4..8........68.........17..9..45...1.5...6.4....9..2.....3...",
    "34.6.......7.......2..8.57......5....7..1..2....4......36.2..1.......9.......7.82",
    "......4.18..2........6.7......8...6..4....3...1.......6......2..5..1....7...3....",
    ".4..5..67...1...4....2.....1..8..3........2...6...........4..5.3.....8..2........",
    ".......4...2..4..1.7..5..9...3..7....4..6....6..1..8...2....1..85.9...6.....8...3",
    "8..7....4.5....6............3.97...8....43..5....2.9....6......2...6...7.71..83.2",
    ".8...4.5....7..3............1..85...6.....2......4....3.26............417........",
    "....7..8...6...5...2...3.61.1...7..2..8..534.2..9.......2......58...6.3.4...1....",
    "......8.16..2........7.5......6...2..1....3...8.......2......7..4..8....5...3....",
    ".2..........6....3.74.8.........3..2.8..4..1.6..5.........1.78.5....9..........4.",
    ".52..68.......7.2.......6....48..9..2..41......1.....8..61..38.....9...63..6..1.9",
    "....1.78.5....9..........4..2..........6....3.74.8.........3..2.8..4..1.6..5.....",
    "1.......3.6.3..7...7...5..121.7...9...7........8.1..2....8.64....9.2..6....4.....",
    "4...7.1....19.46.5.....1......7....2..2.3....847..6....14...8.6.2....3..6...9....",
    "......8.17..2........5.6......7...5..1....3...8.......5......2..3..8....6...4....",
    "963......1....8......2.5....4.8......1....7......3..257......3...9.2.4.7......9..",
    "15.3......7..4.2....4.72.....8.........9..1.8.1..8.79......38...........6....7423",
    "..........5724...98....947...9..3...5..9..12...3.1.9...6....25....56.....7......6",
    "....75....1..2.....4...3...5.....3.2...8...1.......6.....1..48.2........7........",
    "6.....7.3.4.8.................5.4.8.7..2.....1.3.......2.....5.....7.9......1....",
    "....6...4..6.3....1..4..5.77.....8.5...8.....6.8....9...2.9....4....32....97..1..",
    ".32.....58..3.....9.428...1...4...39...6...5.....1.....2...67.8.....4....95....6.",
    "...5.3.......6.7..5.8....1636..2.......4.1.......3...567....2.8..4.7.......2..5..",
    ".5.3.7.4.1.........3.......5.8.3.61....8..5.9.6..1........4...6...6927....2...9..",
    "..5..8..18......9.......78....4.....64....9......53..2.6.........138..5....9.714.",
    "..........72.6.1....51...82.8...13..4.........37.9..1.....238..5.4..9.........79.",
    "...658.....4......12............96.7...3..5....2.8...3..19..8..3.6.....4....473..",
    ".2.3.......6..8.9.83.5........2...8.7.9..5........6..4.......1...1...4.22..7..8.9",
    ".5..9....1.....6.....3.8.....8.4...9514.......3....2..........4.8...6..77..15..6.",
    ".....2.......7...17..3...9.8..7......2.89.6...13..6....9..5.824.....891..........",
    "3...8.......7....51..............36...2..4....7...........6.13..452...........8.."
];
