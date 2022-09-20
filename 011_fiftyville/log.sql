-- Keep a log of any SQL queries you execute as you solve the mystery.

-- Get description of crime
SELECT description
FROM crime_scene_reports
WHERE month = 7
AND day = 28
AND street = "Humphrey Street";

/* Theft of the CS50 duck took place at 10:15am at the Humphrey Street bakery.
Interviews were conducted today with three witnesses who were present at the time – each of their interview transcripts mentions the bakery.
Littering took place at 16:36. No known witnesses. */

-- Get the two transcripts describing the theft
SELECT name, transcript
FROM interviews
WHERE month = 7
AND day = 28;

/*
Eugene:
Sometime within ten minutes of the theft, I saw the thief get into a car in the bakery parking lot and drive away.
If you have security footage from the bakery parking lot, you might want to look for cars that left the parking lot in that time frame.
I don't know the thief's name, but it was someone I recognized.
Earlier this morning, before I arrived at Emma's bakery, I was walking by the ATM on Leggett Street and saw the thief there withdrawing some money.

Raymond:
As the thief was leaving the bakery, they called someone who talked to them for less than a minute.
In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow.
The thief then asked the person on the other end of the phone to purchase the flight ticket.
*/

-- Get details of people that left the bakery between 10:15 and 10:25
SELECT b.hour, b.minute, b.activity, p.license_plate, p.name, p.phone_number, p.passport_number
FROM bakery_security_logs AS b
    JOIN people as p
    ON b.license_plate = p.license_plate

WHERE b.month = 7
AND b.day = 28
AND b.hour = 10
AND b.minute BETWEEN 15 AND 25
AND b.activity = "exit";

/*
+------+--------+----------+---------------+---------+----------------+-----------------+
| hour | minute | activity | license_plate |  name   |  phone_number  | passport_number |
+------+--------+----------+---------------+---------+----------------+-----------------+
| 10   | 16     | exit     | 5P2BI95       | Vanessa | (725) 555-4692 | 2963008352      |
| 10   | 18     | exit     | 94KL13X       | Bruce   | (367) 555-5533 | 5773159633      |
| 10   | 18     | exit     | 6P58WS2       | Barry   | (301) 555-4174 | 7526138472      |
| 10   | 19     | exit     | 4328GD8       | Luca    | (389) 555-5198 | 8496433585      |
| 10   | 20     | exit     | G412CB7       | Sofia   | (130) 555-0289 | 1695452385      |
| 10   | 21     | exit     | L93JTIZ       | Iman    | (829) 555-5269 | 7049073643      |
| 10   | 23     | exit     | 322W7JE       | Diana   | (770) 555-1861 | 3592750733      |
| 10   | 23     | exit     | 0NTHK55       | Kelsey  | (499) 555-9472 | 8294398571      |
+------+--------+----------+---------------+---------+----------------+-----------------+
*/

-- Get names of people that withdrew cash on Leggett Street
SELECT p.name, a.amount, a.atm_location, a.transaction_type
FROM atm_transactions AS a
    JOIN bank_accounts AS b
    ON a.account_number = b.account_number

    JOIN people AS p
    ON b.person_id = p.id

WHERE a.year = 2021
AND a.month = 7
AND a.day = 28
AND transaction_type = "withdraw"
AND atm_location = "Leggett Street";

/*
+---------+--------+----------------+------------------+
|  name   | amount |  atm_location  | transaction_type |
+---------+--------+----------------+------------------+
| Bruce   | 50     | Leggett Street | withdraw         |
| Diana   | 35     | Leggett Street | withdraw         |
| Brooke  | 80     | Leggett Street | withdraw         |
| Kenny   | 20     | Leggett Street | withdraw         |
| Iman    | 20     | Leggett Street | withdraw         |
| Luca    | 48     | Leggett Street | withdraw         |
| Taylor  | 60     | Leggett Street | withdraw         |
| Benista | 30     | Leggett Street | withdraw         |
+---------+--------+----------------+------------------+

This leaves: Bruce, Diana, Iman, Luca
*/

-- Find the potential accomplices of suspects
SELECT p1.name AS caller, p2.name AS receiver, c.duration
FROM phone_calls AS c
    JOIN people AS p1
    ON c.caller = p1.phone_number

    JOIN people AS p2
    ON c.receiver = p2.phone_number

WHERE c.year = 2021
AND c.month = 7
AND c.day = 28
AND c.duration < 60;

/*
+---------+------------+----------+
|  name   |    name    | duration |
+---------+------------+----------+
| Sofia   | Jack       | 51       |
| Kelsey  | Larry      | 36       |
| Bruce   | Robin      | 45       |
| Kelsey  | Melissa    | 50       |
| Taylor  | James      | 43       |
| Diana   | Philip     | 49       |
| Carina  | Jacqueline | 38       |
| Kenny   | Doris      | 55       |
| Benista | Anna       | 54       |
+---------+------------+----------+

This leaves: Bruce, Diana
*/

-- Collect the flight and passenger details
SELECT DISTINCT oa.city AS departure, da.city AS arrival, f.hour, pe.name, pa.seat, f.day
FROM flights AS f
    JOIN airports AS oa
    ON f.origin_airport_id = oa.id
        AND oa.full_name = "Fiftyville Regional Airport"

    JOIN airports AS da
    ON f.destination_airport_id = da.id

    JOIN passengers AS pa
    ON f.id = pa.flight_id

    JOIN people AS pe
    ON pa.passport_number = pe.passport_number

WHERE f.year = 2021
AND f.month = 7
AND f.day = 29
AND hour BETWEEN 6 AND 10;

/*
+------------+---------------+------+---------+------+-----+
| departure  |    arrival    | hour |  name   | seat | day |
+------------+---------------+------+---------+------+-----+
| Fiftyville | New York City | 8    | Doris   | 2A   | 29  |
| Fiftyville | New York City | 8    | Sofia   | 3B   | 29  |
| Fiftyville | New York City | 8    | Bruce   | 4A   | 29  |
| Fiftyville | New York City | 8    | Edward  | 5C   | 29  |
| Fiftyville | New York City | 8    | Kelsey  | 6C   | 29  |
| Fiftyville | New York City | 8    | Taylor  | 6D   | 29  |
| Fiftyville | New York City | 8    | Kenny   | 7A   | 29  |
| Fiftyville | New York City | 8    | Luca    | 7B   | 29  |
| Fiftyville | Chicago       | 9    | Daniel  | 7B   | 29  |
| Fiftyville | Chicago       | 9    | Carol   | 8A   | 29  |
| Fiftyville | Chicago       | 9    | Rebecca | 9A   | 29  |
| Fiftyville | Chicago       | 9    | Sophia  | 2C   | 29  |
| Fiftyville | Chicago       | 9    | Heather | 3B   | 29  |
| Fiftyville | Chicago       | 9    | Marilyn | 4A   | 29  |
+------------+---------------+------+---------+------+-----+

Bruce is the thief and Robin is the accomplice
*/