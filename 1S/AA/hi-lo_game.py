
import random
import statistics

def play_hi_lo_game(number, low, high):
    attempts = 0

    number_choice = 0

    while number_choice != number:
        number_choice = (high + low) // 2
        attempts += 1

        if number_choice < number:
            low = number_choice + 1
        elif number_choice > number:
            high = number_choice - 1

    return attempts 



    

if __name__ == '__main__':
    n = 0
    dict_attempts = {}
    high = 100
    while n < 10000:
        number = random.randint(1, high)

        attempts = play_hi_lo_game(number, 1, high)

        if attempts not in dict_attempts:
            dict_attempts[attempts] = 1
        else:
            dict_attempts[attempts] += 1

        n += 1
        if n % 1000 == 0:
            print(f'Range: 1-{high}')



            # Create list of all attempts for statistical calculations
            all_attempts = []
            total_games = sum(dict_attempts.values())
            
            for attempt_count, frequency in dict_attempts.items():
                all_attempts.extend([attempt_count] * frequency)
            
            # Calculate statistics
            if all_attempts:
                min_attempts = min(all_attempts)
                max_attempts = max(all_attempts)
                mean_attempts = statistics.mean(all_attempts)
                median_attempts = statistics.median(all_attempts)
                
                print(f'Total games played: {total_games}')
                print(f'Min attempts: {min_attempts}')
                print(f'Mean attempts: {mean_attempts:.2f}')
                print(f'Median attempts: {median_attempts:.2f}')
                print(f'Max attempts: {max_attempts}')
                
                # Frequency analysis
                print(f'Frequency distribution (attempts: count, percentage):')
                for attempts_count in sorted(dict_attempts.keys()):
                    count = dict_attempts[attempts_count]
                    percentage = (count / total_games) * 100
                    print(f'  {attempts_count} attempts: {count} times ({percentage:.1f}%)')
            
            print('-' * 50)
            dict_attempts = {}
            high *= 10

