#!/usr/bin/env python3
import re
import datetime

class process_cal:

    def __init__(self, filename):
        self.filename = filename

    ''' Purpose: check events on a day
                 if there are events, 
                 return all events string, 
                 otherwise None
        Parameter: datetime object'''
    def get_events_for_day(self, day):

        # Creating certain patterns to extract substrings
        dtstart_pattern = re.compile(r"DTSTART:((\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)).*")
        dtend_pattern = re.compile(r"DTEND:((\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)).*")
        rrule_pattern = re.compile(r"UNTIL=((\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)).*")
        exdate_pattern = re.compile(r"EXDATE:((\d\d\d\d)(\d\d)(\d\d)T(\d\d)(\d\d)(\d\d)).*")
        location_pattern = re.compile(r"LOCATION:(.*).*")
        summary_pattern = re.compile(r"SUMMARY:(.+).*")

        result = None  #stores returing string
        first_time = 1  #returing string for first event is different than subsequent strings

        # Reading file
        f = open(self.filename, "rt")    
        for line in f:
           # searching dtstart pattern 
           dtstart = dtstart_pattern.search(line)

           if dtstart: #if found
                   # making day_com datetime object with dtstart date 
                   day_com = datetime.datetime(int(dtstart.group(2)), int(dtstart.group(3)), int(dtstart.group(4)))
                   if (day_com == day): #if we find the date
                        # copying full date and time in day_com
                        day_com = datetime.datetime(int(dtstart.group(2)), int(dtstart.group(3)), int(dtstart.group(4)), int(dtstart.group(5)), int(dtstart.group(6)),int(dtstart.group(7)))
                        day_string = day_com.strftime("%B %d, %Y (%a)")  # string needed for result

                        dash_string ="-"
                        for dash in range(0,len(day_string)-1):
                            dash_string +="-"  # string needed for result

                        tstart_string = day_com.strftime("%-2I:%M %p") + " to"    #string needed for result
                        '''Moving to dtend_line,
                           removing extra whitespaces,
                           searching dtend pattern,
                           making end datime object using dtend info,
                           producing tend_string to be used b result '''
                        dtend_line = f.readline()         
                        dtend_line = dtend_line.rstrip()
                        dtend = dtend_pattern.search(dtend_line)
                        end = datetime.datetime(int(dtend.group(2)), int(dtend.group(3)), int(dtend.group(4)), int(dtend.group(5)), int(dtend.group(6)), int(dtend.group(7)))
                        tend_string = end.strftime(" %-2I:%M %p: ") # string needed for result
                        
                        '''Moving to next line that might be rrule or location,
                           removing extra white spaces,
                           if location line, location will be extracted'''
                        rrule_or_location_line = f.readline()
                        rrule_or_location_line = rrule_or_location_line.rstrip()
                        location = location_pattern.search(rrule_or_location_line)
 
                        if location: #if location found

                            place = location.group(1)  #extract place name

                            ''' Moving to summary_line,
                                removing extra spaces,
                                searching pattern,
                                extracting summary in summ variable'''
                            summary_line = f.readline() 
                            summary_line = summary_line.rstrip()
                            summary = summary_pattern.search(summary_line)
                            summ = summary.group(1)

                            if (first_time == 1):   #storing result based on if day found for first_time or more
                                first_time = 0
                                result =  day_string+ "\n" + dash_string + "\n" + tstart_string + tend_string + summ + " {{" + place + "}}"
                            else:
                                result = result + "\n" + tstart_string + tend_string + summ + " {{" + place + "}}"
                   else:     # If (day_com!=day) day to compare is not equal to day we are interested in

                        ''' Reading next line : dtend_line,
                            removing extra spaces,
                            searching dtend pattern,
                            extracting end datetime object from dtend'''
                        dtend_line = f.readline()
                        dtend_line = dtend_line.rstrip()
                        dtend = dtend_pattern.search(dtend_line)
                        end = datetime.datetime(int(dtend.group(2)), int(dtend.group(3)), int(dtend.group(4)), int(dtend.group(5)), int(dtend.group(6)), int(dtend.group(7)))

                        ''' Moving to next line : which is either rrule or location line
                            removing extra spaces,
                            if rrule line, extracting rrule'''
                        rrule_or_location_line = f.readline()
                        rrule_or_location_line = rrule_or_location_line.rstrip()
                        rrule = rrule_pattern.search(rrule_or_location_line)

                        if rrule:    #if rrule found

                            until = datetime.datetime(int(rrule.group(2)), int(rrule.group(3)),int(rrule.group(4)), int(rrule.group(5)), int(rrule.group(6)), int(rrule.group(7))) # extracting until date
                            
                            ''' Moving to next line: which is either exdate or location line
                                removing extra spaces,
                                if location line, search location
                                if exdate line, search exdate '''
                            location_or_exdate_line = f.readline()
                            location_or_exdate_line = location_or_exdate_line.rstrip()
                            location = location_pattern.search(location_or_exdate_line)
                            exdate = exdate_pattern.search(location_or_exdate_line)

                            if location:   # if location found

                                place = location.group(1)  # extracting place name

                                ''' Next line is summary line,
                                    extracting summary'''
                                summary_line = f.readline()  
                                summary_line = summary_line.rstrip()
                                summary = summary_pattern.search(summary_line)
                                summ = summary.group(1)

                                ''' Condition: day to compare less than or equal to until
                                    incrementing day_com and end by 7 days'''
                                while (day_com<=until):
                                    if(day_com == day):    # if day found
                                        day_compare = datetime.datetime(int(dtstart.group(2)), int(dtstart.group(3)), int(dtstart.group(4)), int(dtstart.group(5)), int(dtstart.group(6)),int(dtstart.group(7)))
                                        day_string = day_com.strftime("%B %d, %Y (%a)")  #needed for result
                                        dash_string ="-"
                                        for dash in range(0,len(day_string)-1):
                                            dash_string +="-"  #needed for result
                                        tstart_string = day_compare.strftime("%-2I:%M %p") + " to" #needed for result
                                        tend_string = end.strftime(" %-2I:%M %p: ") #needed for result
                                        if first_time == 1:  #if first time day is found then result is different
                                            first_time = 0
                                            result = day_string+ "\n" + dash_string + "\n" + tstart_string + tend_string + summ + " {{" + place + "}}"
                                        else:
                                            result = result + "\n" + tstart_string + tend_string + summ + " {{" + place + "}}"

                                    delta = datetime.timedelta(7)  # delta = 7 days
                                    day_com = day_com + delta    # incrementing day_com
                                    end = end + delta            # incrementing end

                            if exdate:  # if exdate found
                                exdate = datetime.datetime(int(exdate.group(2)), int(exdate.group(3)), int(exdate.group(4)), int(exdate.group(5)), int(exdate.group(6)), int(exdate.group(7))) # extracting exdate
                                
                                ''' Moving to location_line,
                                    removing extra spaces,
                                    extracting place name '''
                                location_line = f.readline()
                                location_line = location_line.rstrip()
                                location = location_pattern.search(location_line)
                                place = location.group(1)

                                ''' Moving to summary_line,
                                    removing extra spaces,
                                    extracting summary and transfering it into summ'''
                                summary_line = f.readline()
                                summary_line = summary_line.rstrip()
                                summary = summary_pattern.search(summary_line)
                                summ = summary.group(1)

                                ''' Condition: day to compare less than or equal to until
                                    incrementing day_com and end by 7 days'''
                                while (day_com<=until):   
                                    if(day_com == day):  # if day found
                                        day_compare = datetime.datetime(int(dtstart.group(2)), int(dtstart.group(3)), int(dtstart.group(4)), int(dtstart.group(5)), int(dtstart.group(6)),int(dtstart.group(7)))
                                        if(day_compare!=exdate):  # skipping over exdate and only taking result if datetime is not exdate
                                            day_string = day_com.strftime("%B %d, %Y (%a)")
                                            dash_string ="-"
                                            for dash in range(0,len(day_string)-1):
                                                dash_string +="-"
                                            tstart_string = day_compare.strftime("%-2I:%M %p") + " to"
                                            tend_string = end.strftime(" %-2I:%M %p: ")
                                            if first_time == 1:
                                                first_time = 0
                                                result = day_string+ "\n" + dash_string + "\n" + tstart_string + tend_string + summ + " {{" + place + "}}"
                                            else:
                                                result = result + "\n" + tstart_string + tend_string + summ + " {{" + place + "}}"
                                    delta = datetime.timedelta(7) # delta = 7 days
                                    day_com = day_com + delta  # incrementing day_com
                                    end = end + delta  # incrementing end
        f.close()
        return result
