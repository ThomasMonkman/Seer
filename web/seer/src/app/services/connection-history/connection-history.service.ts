import { Injectable } from '@angular/core';
import { CookieService } from 'angular2-cookie/services/cookies.service';
import { Observable } from 'rxjs/Observable';
import { ReplaySubject } from 'rxjs/ReplaySubject';
import { ConnectionMeta } from "app/classes/connnection-meta";

@Injectable()
export class ConnectionHistoryService {
  private readonly cookieKey: string = 'connectionHistory';
  private connections: ConnectionMeta[] = [];
  private readonly connectionsSubject: ReplaySubject<ConnectionMeta[]> = new ReplaySubject(1);
  private readonly defaultConnection: ConnectionMeta = new ConnectionMeta('localhost', 'localhost');
  constructor(private cookieService: CookieService) {
    const connectionCookie: string = this.cookieService.get(this.cookieKey);
    if(typeof connectionCookie === 'undefined'){
      //first time using this cookie
      this.store(this.defaultConnection);
      this.connections.push(this.defaultConnection);
    }else{
      this.connections = JSON.parse(connectionCookie);
    }
    this.connectionsSubject.next(this.connections);
  }

  public get(): Observable<ConnectionMeta[]> {
    return this.connectionsSubject.share();
  }

  public store(connection: ConnectionMeta) {
    //Only store if not already in history
    const alreadyStored = this.connections.some(stored => stored.address === connection.address);
    if (alreadyStored === false) {
      this.connections.push(connection);
      this.cookieService.put(this.cookieKey, JSON.stringify(this.connections));
      this.connectionsSubject.next(this.connections);
    }
  }

  public remove(name: string) {
    this.connections = this.connections.filter(connection => connection.name === name);
    this.cookieService.put(this.cookieKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }
}
